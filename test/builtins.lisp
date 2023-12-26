(define x 5)                         ;expect: x
(+ 1 2)                              ;expect: 3
(+ x (+ 3 9))                        ;expect: 17
(define x (+ 9 4))                   ;expect: x
x                                    ;expect: 13
(- 7)                                ;expect: -7
(- 7 5)                              ;expect: 2
(- 7 5 1)                            ;expect: 1
(*)                                  ;expect: 1
(* 5 7)                              ;expect: 35
(* x (+ 1 2))                        ;expect: 39
(/ 1)                                ;expect: 1
(/ 256 8)                            ;expect: 32
(/ 256 4 2)                          ;expect: 32
(lambda (x) (* x 2))                 ;expect: (lambda (x) (* x 2))
(define double (lambda (x) (* x 2))) ;expect: double
(car '(a b c))                       ;expect: a
(cdr '(x y z))                       ;expect: (y z)
(cons 'p 'q)                         ;expect: (p . q)
(cons 'a '(b c d))                   ;expect: (a b c d)
(or)                                 ;expect: #f
(or #f)                              ;expect: #f
(or #t)                              ;expect: #t
(or #t #f)                           ;expect: #t
(or #f #f)                           ;expect: #f
(and)                                ;expect: #t
(and #f)                             ;expect: #f
(and #t)                             ;expect: #t
(and #t #t)                          ;expect: #t
(and #f #t)                          ;expect: #f
(<)                                  ;expect: #t
(< 2)                                ;expect: #t
(< 1 2 3 4 5)                        ;expect: #t
(< 7 2 6 8)                          ;expect: #f
(>)                                  ;expect: #t
(> 2)                                ;expect: #t
(> 5 4 3 2 1)                        ;expect: #t
(> 7 2 6 8)                          ;expect: #f
(=)                                  ;expect: #t
(= 7 1)                              ;expect: #f
(define z 12)                        ;expect: z
(= 12 z)                             ;expect: #t
(= z z z z)                          ;expect: #t
