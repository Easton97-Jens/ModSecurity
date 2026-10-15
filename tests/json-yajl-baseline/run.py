#!/usr/bin/env python3
"""v2 real-Apache characterization, explicit expectations, byte-safe observation.

Requires built mod_security2.so WITH_YAJL and Apache 2.4. Uses one bounded server,
synthetic requests, and existing rule/debug interfaces. No production hooks.
"""
import argparse
import collections
import hashlib
import http.client
import json
import os
from pathlib import Path
import re
import socket
import subprocess
import sys
import time
from cases import CASES

COLLECTIONS = ['ARGS', 'ARGS_NAMES', 'ARGS_POST', 'ARGS_GET', 'REQBODY_ERROR', 'REQBODY_ERROR_MSG', 'REQBODY_PROCESSOR', '&ARGS','INBOUND_DATA_ERROR']

def decode_escaped(text):
    # Existing log_escape_nq_ex uses printable ASCII and C escapes; names can
    # contain quote, slash, Unicode. ARGS_NAMES values are observed as hex too.
    out=bytearray(); i=0
    while i<len(text):
        if text[i]=='\\' and i+1<len(text):
            if text[i+1]=='x' and i+3<len(text):out.append(int(text[i+2:i+4],16));i+=4;continue
            chars={'n':10,'r':13,'t':9,'b':8,'f':12,'v':11,'\\':92,'"':34}
            if text[i+1] in chars:out.append(chars[text[i+1]]);i+=2;continue
        out.extend(text[i].encode());i+=1
    return bytes(out)

def observe(log):
    result={k:[] for k in COLLECTIONS}; section=None; name=None
    for line in log.splitlines():
        if 'Recipe: Invoking rule' in line:
            m=re.search(r'\[id "(\d+)"\]',line);section=None if not m else int(m[1])%100
        if section is not None and 10<=section<10+len(COLLECTIONS):
            m=re.search(r'Executing operator .* against (.*)\.$',line)
            if m:name=m[1]
            m=re.search(r'Target value: "([0-9a-f]*)"$',line)
            if m:
                col=COLLECTIONS[section-10]
                key=(name or '').split(':',1)[1] if ':' in (name or '') else ''
                result[col].append((decode_escaped(key),bytes.fromhex(m[1])))
    return result

def rule_matched(log, target):
    active=None
    for line in log.splitlines():
        if 'Recipe: Invoking rule' in line:
            m=re.search(r'\[id "(\d+)"\]',line)
            active=int(m[1]) if m else None
        if active==target and 'Rule returned 1.' in line:
            return True
    return False

def config(args, selected, port, out):
    module=Path(args.module).resolve(); modules=Path(args.modules).resolve()
    lines=[f'ServerRoot "{out}"',f'PidFile "{out}/httpd.pid"',f'Listen 127.0.0.1:{port}','ServerName localhost',f'ErrorLog "{out}/error.log"','LogLevel warn','User #'+str(os.getuid()),'Group #'+str(os.getgid())]
    for name in ['mpm_event','authn_core','authz_core','unixd','unique_id']:
        if (modules/f'mod_{name}.so').exists():lines.append(f'LoadModule {name}_module "{modules}/mod_{name}.so"')
    lines += [f'LoadModule security2_module "{module}"',f'DocumentRoot "{out}/htdocs"',f'<Directory "{out}/htdocs">','Require all granted','</Directory>','SecRuleEngine On','SecRequestBodyAccess On','SecRequestBodyLimitAction ProcessPartial',f'SecTmpDir "{out}"',f'SecDataDir "{out}"',f'SecUploadDir "{out}"',f'SecDebugLog "{out}/debug.log"','SecDebugLogLevel 9','SecAuditEngine Off']
    for i,c in enumerate(selected):
        base=(i+1)*100
        lines += [f'<Location /{c["id"]}>', 'SecRequestBodyAccess '+('On' if c.get('access',True) else 'Off'),*([f'SecRequestBodyJsonDepthLimit {c.get("depth",10000)}'] if not c.get('default_depth') else []),f'SecArgumentsLimit {c.get("limit",1000)}','SecRequestBodyLimitAction '+c.get('action','ProcessPartial'),'SecRuleEngine '+c.get('mode','On')]
        if c.get('select',True):
            lines.insert(20, f'SecRule REQUEST_URI "@beginsWith /{c["id"]}" "id:{base+1},phase:1,t:none,pass,nolog,ctl:requestBodyProcessor=JSON"')
        if c.get('body_limit'):
            lines.insert(20, f'SecRule REQUEST_URI "@beginsWith /{c["id"]}" "id:{base+2},phase:1,t:none,pass,nolog,ctl:requestBodyLimit={c["body_limit"]}"')
        for j,col in enumerate(COLLECTIONS):
            lines.append(f'SecRule {col} "@rx ^z" "id:{base+10+j},phase:2,t:none,t:hexEncode,pass,nolog"')
        if c.get('deny_error'):
            lines.append(f'SecRule REQBODY_ERROR "@eq 1" "id:{base+31},phase:2,t:none,deny,status:403,log,msg:EXPLICIT_ERROR_DENIAL"')
        if c.get('exact_rule'):
            token=c['args'][0][1].hex()+('00' if args.negative_control and c is selected[1] and args.negative_control_field=='value' else '')
            lines.append(f'SecRule ARGS "@streq {token}" "id:{base+30},phase:2,t:none,t:hexEncode,pass,log,msg:EXACT_VALUE_CONFIRMED"')
        lines.append('</Location>')
    return '\n'.join(lines)+'\n'

def expected(c):
    args=c['args']; query=[]
    if c.get('query'):query=[(b'q',b'v')]
    # v2 JSON arguments have origin JSON, while ARGS_POST filters origin BODY.
    return {'ARGS':args,'ARGS_NAMES':[(n,n) for n,v in args], 'ARGS_POST':[], 'ARGS_GET':query,
            'REQBODY_ERROR':[(b'',str(c['error']).encode())], '&ARGS':[(b'',str(len(args)).encode())], 'INBOUND_DATA_ERROR':[(b'',str(c.get('inbound',0)).encode())]}

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--httpd',required=True);p.add_argument('--modules',required=True);p.add_argument('--module',required=True);p.add_argument('--output',required=True);p.add_argument('--filter',default='');p.add_argument('--negative-control',action='store_true');p.add_argument('--negative-control-field',choices=['value','name','count'],default='value');opts=p.parse_args()
    selected=[c for c in CASES if opts.filter in c['id']]
    if not selected:p.error('filter selects zero cases')
    selected.insert(0,dict(id='CANARY-json-enabled',body=b'{"canary":"enabled"}',args=[(b'canary',b'enabled')],error=0))
    out=Path(opts.output).resolve();out.mkdir(parents=True,exist_ok=False);(out/'htdocs').mkdir()
    for c in selected:(out/'htdocs'/c['id']).write_bytes(b'ok\n')
    with socket.socket() as s:s.bind(('127.0.0.1',0));port=s.getsockname()[1]
    conf=out/'httpd.conf';conf.write_text(config(opts,selected,port,out))
    with (out/'startup.log').open('wb') as log:
        server=subprocess.Popen([opts.httpd,'-X','-f',str(conf)],stdout=log,stderr=log)
    results=[]
    try:
        for _ in range(100):
            if server.poll() is not None:raise RuntimeError('Apache startup failed: '+(out/'startup.log').read_text())
            try:
                with socket.create_connection(('127.0.0.1',port),timeout=.1):break
            except OSError:time.sleep(.05)
        else:raise RuntimeError('Apache startup timeout')
        for c in selected:
            body=c['body']; path='/'+c['id']+('?' + c['query'] if c.get('query') else '')
            before=(out/'debug.log').stat().st_size if (out/'debug.log').exists() else 0
            conn=http.client.HTTPConnection('127.0.0.1',port,timeout=8)
            conn.putrequest('POST',path);conn.putheader('Content-Type',c.get('content_type','application/json'))
            if body is not None:conn.putheader('Content-Length',str(len(body)))
            conn.endheaders(body);response=conn.getresponse();status=response.status;response.read();conn.close()
            log=(out/'debug.log').read_bytes()[before:].decode('utf-8','backslashreplace');(out/(c['id']+'.log')).write_text(log)
            exact_matched=rule_matched(log,(selected.index(c)+1)*100+30) if c.get('exact_rule') else False
            got=observe(log);want=expected(c)
            if not c.get('phase2',True):
                want={k:[] for k in want}
            if c.get('exact_rule'):
                # Debug logging truncates long values. Preserve the observation;
                # an independent byte-exact streq rule plus ARGS_NAMES and
                # &ARGS assertions prove this case without fabricating output.
                want.pop('ARGS')
            if opts.negative_control and c is selected[1]:
                if opts.negative_control_field=='name':want['ARGS_NAMES']=[(b'wrong',b'wrong')]
                elif opts.negative_control_field=='count':want['&ARGS']=[(b'',b'0')]
                elif not c.get('exact_rule'):want['ARGS']=[(b'wrong',b'wrong')]
            failures=[]
            if status!=c.get('http',200):failures.append('HTTP status '+str(status))
            if 'JSON support was not enabled' in log:failures.append('YAJL disabled')
            for col, entries in want.items():
                if got[col]!=entries:failures.append(col+' expected '+repr(entries)+' got '+repr(got[col]))
            invoked='JSON parser initialization' in log
            if invoked!=c.get('parser',True):failures.append('parser invocation expected '+str(c.get('parser',True))+' got '+str(invoked))
            if c['error'] and c.get('phase2',True):
                messages=[v for n,v in got['REQBODY_ERROR_MSG']]
                prefix=(b'JSON parsing error:' if c.get('deny_error') else b'JSON parsing error: JSON depth limit exceeded' if c['id'].startswith('E-') else b'JSON parsing error: More than ' if c['id'].startswith('F-') else b'JSON parsing error: lexical error:' if c['id'].startswith('G-') else b'JSON parsing error:')
                if len(messages)!=1 or not messages[0].startswith(prefix):failures.append('unexpected error class '+repr(messages))
            if c['error'] and not c.get('phase2',True) and c.get('complete_error','JSON parser error: parse error: premature EOF') not in log:failures.append('missing completion-specific EOF error')
            if c.get('exact_rule') and not exact_matched:failures.append('exact value rule did not match')
            if not c['error'] and got['REQBODY_ERROR_MSG'] and got['REQBODY_ERROR_MSG'][0][1]:failures.append('unexpected error message')
            if c.get('body_reject') and 'Request body (Content-Length) is larger than the configured limit (8)' not in log:failures.append('missing body-limit rejection evidence')
            if c.get('deny_error') and 'EXPLICIT_ERROR_DENIAL' not in log:failures.append('missing explicit denial rule evidence')
            if c.get('phase2',True) and c.get('select',True) and got['REQBODY_PROCESSOR']!=[(b'',b'JSON')]:failures.append('processor selection not JSON')
            results.append(dict(id=c['id'],status='FAIL' if failures else 'PASS',failures=failures,body_length=None if body is None else len(body),body_hex=None if body is None else body.hex(),body_sha256=None if body is None else hashlib.sha256(body).hexdigest(),expected_args=[dict(name_hex=n.hex(),value_hex=v.hex()) for n,v in c['args']],expected_error=c['error'],phase2_expected=c.get('phase2',True),http_expected=c.get('http',200),http_actual=status,args=[dict(name_hex=n.hex(),value_hex=v.hex()) for n,v in got['ARGS']],error=[v.decode() for n,v in got['REQBODY_ERROR']],parser_invoked=invoked,exact_value_rule_matched=exact_matched if c.get('exact_rule') else None))
            print(results[-1]['status'],c['id'],*failures,sep=' | ')
    finally:
        server.terminate()
        try:server.wait(timeout=5)
        except subprocess.TimeoutExpired:server.kill();server.wait()
    counts=dict(collections.Counter(r['status'] for r in results));(out/'results.json').write_text(json.dumps(dict(counts=counts,cases=results),indent=2)+'\n');print(json.dumps(counts));return 1 if counts.get('FAIL') else 0

if __name__=='__main__':
    try:sys.exit(main())
    except (RuntimeError,OSError,http.client.HTTPException) as exc:print('BLOCKED:',exc,file=sys.stderr);sys.exit(2)
