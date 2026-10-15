"""Explicit byte fixtures and v2 characterization expectations (not desired policy).

Never parse/re-serialize request bodies. Entries are ordered (name, value) lists;
repeated names remain repeated. Cases contain synthetic data only.
"""
CASES = []

def case(id, body, args=(), error=0, **options):
    if isinstance(body, str):
        body = body.encode('utf-8')
    entries = [(n.encode() if isinstance(n, str) else n,
                v.encode() if isinstance(v, str) else v) for n, v in args]
    CASES.append(dict(id=id, body=body, args=entries, error=error, **options))

case('A-no-body', None, parser=False)
case('A-zero-bytes', b'')
case('A-whitespace', b' \t\r\n', error=1, http=500, phase2=False)
case('A-object', '{}')
case('A-array', '[]')
case('A-empty-string', '""', [('', '')])
case('B-empty', '{"":"value"}', [('', 'value')])
case('B-nested-empty', '{"a":{"":"v"},"":"z"}', [('a.', 'v'),('', 'z')])
case('B-empty-siblings', '{"":"x","":"y"}', [('', 'x'),('', 'y')])
case('B-dot-flat', '{"a.b":1}', [('a.b','1')])
case('B-dot-nested', '{"a":{"b":1}}', [('a.b','1')])
case('B-backslash-unicode', '{"a\\\\b":"x","ö":"💩"}', [('a\\b','x'),('ö','💩')])
case('B-nul-key', b'{"a\\u0000b":"x"}', [('a','x')])
for id, token, value in [('string','"hello"','hello'),('empty','""',''),('int','25','25'),('decimal','1.2300','1.2300'),('true','true','true'),('false','false','false'),('null','null','')]:
    case('C-'+id,token,[('',value)])
    case('C-'+id+'-ws',' \t'+token+'\r\n',[('',value)])
case('D-root-array','[1,2]', [('array.array','1'),('array.array','2')])
case('D-object-array','{"a":[1,2],"z":3}', [('a.a','1'),('a.a','2'),('z','3')])
case('D-objects','[{"x":1},{"x":2}]',[('array.array.x','1'),('array.array.x','2')])
case('D-multidimensional','[[1,2],[3]]',[('array.array.array','1'),('array.array.array','2'),('array.array.array','3')])
case('D-mixed-empty','{"a":[{},1,[],2,{"b":[3]},4,{},[]],"z":5}', [('a.a','1'),('a.a','2'),('a.a.b.b','3'),('a.a','4'),('z','5')])
# v2 does not count the root object, but does count the root array.
for d in (2,3,4):
    body='{"a":'*d+'1'+'}'*d
    case('E-object-'+str(d),body,[('.'.join(['a']*d),'1')],depth=3)
    body='['*d+'1'+']'*d
    case('E-array-'+str(d),body,[] if d>3 else [('.'.join(['array']*(d+1)),'1')],error=int(d>3),depth=3)
    body='{"a":'+'['*d+'1'+']'*d+'}'
    case('E-mixed-'+str(d),body,[] if d>3 else [('.'.join(['a']*(d+1)),'1')],error=int(d>3),depth=3)
case('E-object-over','{"a":{"b":{"c":{"d":{"e":1}}}}}',error=1,depth=3)
case('E-partial','{"ok":1,"a":[[[[2]]]]}', [('ok','1')],1,depth=3)
case('E-limit-one-object','{"a":1}', [('a','1')],depth=1)
case('E-limit-one-array','[[1]]',error=1,depth=1)
for n in (2,3,4):
    values=['"s"','2','true','null'][:n]
    args=[('k'+str(i),v) for i,v in enumerate(['s','2','true',''][:min(n,3)])]
    case('F-object-'+str(n),'{'+','.join('"k%d":%s'%(i,v) for i,v in enumerate(values))+'}',args,int(n>3),limit=3)
    case('F-array-'+str(n),'['+','.join(values)+']',[('array.array',v) for _,v in args],int(n>3),limit=3)
for token in ('false','true','null','"x"','4'):
    case('F-callback-'+token.replace('"',''),'{"a":1,"b":'+token+'}', [('a','1')],1,limit=1)
case('F-empty-containers','{"a":{},"b":[],"c":1}', [('c','1')],limit=1)
case('F-duplicates','{"a":1,"a":2,"a":3,"a":4}', [('a','1'),('a','2'),('a','3')],1,limit=3)
case('F-query-json','{"j":1,"k":2}', [('q','v'),('j','1')],1,limit=2,query='q=v')
case('F-query-at','{"j":1}', [('q','v'),('j','1')],limit=2,query='q=v')
case('G-utf8','{"¢":"¢","€":"€","💩":"💩"}', [('¢','¢'),('€','€'),('💩','💩')])
case('G-escapes',b'{"\\u00a2":"\\u20ac","\\ud83d\\udca9":"\\ud83d\\udca9"}', [('¢','€'),('💩','💩')])
case('G-control-escapes',b'{"x":"\\b\\f\\n\\r\\t\\\\\\\"\\/"}', [('x',b'\b\f\n\r\t\\"/')])
case('G-nul-value',b'{"x":"a\\u0000b"}', [('x',b'a\0b')])
for id,raw in [('continuation',b'\x80'),('overlong',b'\xc0\xaf'),('truncated',b'\xe2\x82'),('range',b'\xf4\x90\x80\x80')]:
    case('G-invalid-value-'+id,b'{"x":"'+raw+b'"}', [('x',raw)] if id in ('overlong','range') else [], error=int(id not in ('overlong','range')))
    case('G-invalid-key-'+id,b'{"'+raw+b'":"x"}', [(raw,'x')] if id in ('overlong','range') else [], error=int(id not in ('overlong','range')))
for id,raw in [('escape',b'\\q'),('short-u',b'\\u123'),('bad-u',b'\\uZZZZ'),('raw-control',b'\x01')]:
    case('G-invalid-'+id,b'{"x":"'+raw+b'"}',error=1)
case('G-high-surrogate',b'{"x":"\\ud800"}', [('x','?')])
case('G-low-surrogate',b'{"x":"\\udc00"}', [('x',b'\xed\xb0\x80')])
numbers=['0','-0','1','-1','-9223372036854775808','9223372036854775807','-9223372036854775809','9223372036854775808','18446744073709551615','18446744073709551616','9007199254740991','9007199254740992','9007199254740993','1.0','1.2300','1e2','1E+02','1e-2','1e400','1e-400','9'*96]
for i,n in enumerate(numbers):case('H-number-%02d'%i,'{"n":'+n+'}', [('n',n)])
for i,n in enumerate(['01','+1','1.','1e','NaN','Infinity']):case('H-invalid-%02d'%i,'{"n":'+n+'}', [('n','0')] if n=='01' else [],1)
case('I-same','{"a":"x","a":"x"}', [('a','x'),('a','x')])
case('I-different','{"a":"x","a":"y"}', [('a','x'),('a','y')])
case('I-empty','{"":1,"":2}', [('','1'),('','2')])
case('I-nested','{"x":{"a":1,"a":2},"y":{"a":3}}', [('x.a','1'),('x.a','2'),('y.a','3')])
case('I-collision','{"a.b":1,"a":{"b":2}}', [('a.b','1'),('a.b','2')])
case('J-follow-ws','{"a":1} \t\r\n', [('a','1')])
case('J-garbage','{"a":1}x', [('a','1')],1)
case('J-documents','{"a":1}{"b":2}', [('a','1')],1)
for id,body,args in [('object','{"a":1',[('a','1')]),('array','[1,',[('array.array','1')]),('string','{"x":"abc',[]),('number','1e',[]),('literal','tru',[])]:case('J-truncated-'+id,body,args,1,http=500,phase2=False,complete_error=('JSON parser error: lexical error: malformed number' if id=='number' else 'JSON parser error: lexical error: invalid string' if id=='literal' else 'JSON parser error: parse error: premature EOF'))
case('K-access-off','{"a":1}',access=False,parser=False)
case('K-no-selection','{"a":1}',select=False,parser=False)
case('K-type-plain','{"a":1}', [('a','1')],content_type='text/plain')
case('K-type-suffix','{"a":1}', [('a','1')],content_type='application/problem+json')
case('K-type-charset','{"a":1}', [('a','1')],content_type='application/json; charset=UTF-8')
case('K-after-error','{"a":1}', [('a','1')])
case('K-long','{"'+('k'*256)+'":"'+('v'*2048)+'"}', [('k'*256,'v'*2048)],exact_rule=True)
case('K-large','['+','.join(str(i) for i in range(256))+']', [('array.array',str(i)) for i in range(256)])
case('K-audit','{"q":"\\\"\\\\\\n¢€💩"}', [('q','"\\\n¢€💩')])

case('D-walkthrough','{"items":[["A","B"],"C"],"tail":"D"}', [('items.items.items','A'),('items.items.items','B'),('items.items','C'),('tail','D')])
# The processor API correctly rejects an empty JSON text; Apache ignores that
# completion result for a zero-byte body. Explicit per-layer expectation.
next(c for c in CASES if c['id']=='A-zero-bytes')['processor_error']=1
for mode in ('On','DetectionOnly'):
    for action in ('Reject','ProcessPartial'):
        for n in (7,8,9):
            over=n>8
            reject=over and mode=='On' and action=='Reject'
            case('K-body-%s-%s-%d'%(mode,action,n),'\u007b"a":1\u007d'+' '*(n-7), [] if reject else [('a','1')], body_limit=8,action=action,mode=mode,http=413 if reject else 200,phase2=not reject,parser=not reject,body_reject=reject,inbound=int(over and not reject))
case('E-default-depth-64','['*64+'1'+']'*64,[('.'.join(['array']*65),'1')],default_depth=True)
case('K-error-deny','{"a":1}x',[('a','1')],1,deny_error=True,http=403)
case('K-intro-object','{"user":"Ada","enabled":true,"count":1.2300,"optional":null}', [('user','Ada'),('enabled','true'),('count','1.2300'),('optional','')])
case('F-query-below','{}',[('q','v')],limit=2,query='q=v')
