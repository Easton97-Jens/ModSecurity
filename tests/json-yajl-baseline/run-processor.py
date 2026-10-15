#!/usr/bin/env python3
"""Supplementary ModSecurity processor API tests: real parser chunks, no HTTP.

The helper calls unchanged json_init/json_process_chunk/json_complete/cleanup.
Logging is silenced; argument escaping uses unchanged msc_util.c. The integration
suite separately covers Apache, processor selection, error propagation and rules.
"""
import argparse
from collections import Counter
import json
from pathlib import Path
import subprocess
from cases import CASES

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--processor',required=True);p.add_argument('--output',required=True);p.add_argument('--filter',default='');p.add_argument('--negative-control',action='store_true');a=p.parse_args()
    cases=[c for c in CASES if a.filter in c['id'] and c['body'] is not None and c.get('parser',True) and not c.get('query') and not c.get('body_limit')]
    if not cases:p.error('filter selects zero processor cases')
    results=[]
    for c in cases:
        for chunk in sorted({1,2,3,7,max(1,len(c['body']))}):
            r=subprocess.run([a.processor,str(chunk),str(c.get('depth',10000)),str(c.get('limit',1000))],input=c['body'],capture_output=True,timeout=8)
            errors=[];args=[];state=None
            for line in (r.stdout.splitlines() if r.returncode==0 else []):
                if line.startswith(b'state '):state=list(map(int,line.split()[1:]))
                if line.startswith(b'arg '):n,v=line[4:].split(b' ',1);args.append((bytes.fromhex(n.decode()),bytes.fromhex(v.decode())))
            if b'runtime error:' in r.stderr or b'ERROR: AddressSanitizer' in r.stderr:errors.append('sanitizer diagnostic '+r.stderr.decode(errors='backslashreplace')[:500])
            if r.returncode:errors.append('helper exit '+str(r.returncode)+' '+r.stderr.decode(errors='backslashreplace')[:500])
            if state is None:errors.append('missing state')
            else:
                failed=state[0]<0 or state[1]<0
                if failed!=bool(c.get('processor_error',c['error'])):errors.append('wrong parse outcome '+str(state))
                if c.get('phase2') is False and not (state[0]==1 and state[1]==-1):errors.append('expected completion-only error '+str(state))
                if c['id'].startswith('E-') and c['error'] and state[3]!=1:errors.append('depth flag absent')
                if c['id'].startswith('F-') and c['error'] and state[2]!=1:errors.append('argument-limit flag absent')
            expected=c['args']
            if a.negative_control and c is cases[0] and chunk==1:expected=[(b'wrong',b'wrong')]
            if args!=expected:errors.append('arguments expected '+repr(expected)+' got '+repr(args))
            row=dict(id=c['id']+'/chunk-'+str(chunk),status='FAIL' if errors else 'PASS',failures=errors,state=state,args=[dict(name_hex=n.hex(),value_hex=v.hex()) for n,v in args]);results.append(row);print(row['status'],row['id'],*errors,sep=' | ')
    counts=dict(Counter(r['status'] for r in results));Path(a.output).write_text(json.dumps(dict(counts=counts,cases=results),indent=2)+'\n');print(counts);return int(bool(counts.get('FAIL')))
if __name__=='__main__':raise SystemExit(main())
