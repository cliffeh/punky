()                  ;expect: ()
#t                  ;expect: #t
#f                  ;expect: #f
369                 ;expect: 369
"Hello, world!"     ;expect: "Hello, world!"
'a                  ;expect: a
'query?             ;expect: query?
'123                ;expect: 123
'()                 ;expect: ()
''"Lorem ipsum est" ;expect: (quote "Lorem ipsum est")
'(x . 9)            ;expect: (x . 9)
'(foo bar baz)      ;expect: (foo bar baz)
