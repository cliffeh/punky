(define double (lambda (x) (* x 2))) ;expect: double
(double 12)                          ;expect: 24
(double (double 15))                 ;expect: 60
((lambda (x) (+ x 7)) 5)             ;expect: 12
