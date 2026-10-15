#!/usr/bin/env python3
"""v2 Apache configuration acceptance tests (not runtime parser cases)."""
import argparse
from collections import Counter
import json
from pathlib import Path
import subprocess
from run import config

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--httpd',required=True);p.add_argument('--modules',required=True);p.add_argument('--module',required=True);p.add_argument('--output',required=True);a=p.parse_args()
    out=Path(a.output).resolve();out.mkdir(parents=True,exist_ok=False);(out/'htdocs').mkdir();results=[]
    for directive in ('SecRequestBodyJsonDepthLimit','SecArgumentsLimit'):
        for value,accepted in [('1',True),('3',True),('0',False),('-1',False),('invalid',False),('3trailing',True),('9223372036854775807',False)]:
            conf=out/(directive+'-'+value+'.conf');conf.write_text(config(a,[],18089,out)+directive+' '+value+'\n')
            r=subprocess.run([a.httpd,'-t','-f',str(conf)],capture_output=True,text=True,timeout=5)
            specific='Invalid value for '+directive in r.stderr
            blocked=('alloc_listener' in r.stderr and 'Operation not permitted' in r.stderr)
            passed=(r.returncode==0 if accepted else r.returncode==1 and specific)
            row=dict(id='CFG-'+directive+'-'+value,expected_accepted=accepted,exit=r.returncode,status='BLOCKED' if blocked else 'PASS' if passed else 'FAIL',diagnostic=r.stderr.strip());results.append(row);print(row['status'],row['id'])
    counts=dict(Counter(r['status'] for r in results));(out/'results.json').write_text(json.dumps(dict(counts=counts,cases=results),indent=2)+'\n');print(counts);return 2 if counts.get('BLOCKED') else int(bool(counts.get('FAIL')))
if __name__=='__main__':raise SystemExit(main())
