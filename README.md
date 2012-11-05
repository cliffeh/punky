punky - a simple functional language interpreter

BUILDING

make

RUNNING

./punky

WHAT WORKS

* simple integer and floating-point arithmetic; e.g.,

  (+ 1 2)
  (* 5 7)
  (+ 4 (* 6 3) 5)

* variable and function definition; e.g.,

  (define foo 27)
  (define double (lambda (x) (* x 2)))

* quote, car, cdr, cons, list; e.g.,

  (car '(a b c d))
  (list 'a 'b 'c 'd)
  (cons 1 '(2 3 4))

* dot notation; e.g.,

  (1 . 2)

* let; e.g.,

  (let ((x 10) (y 20)) (+ x y))

* string operations: substr

* simple boolean operations; e.g.,

  (not #t)
  (and #f #t)
  (or #t #f)

* if statements; e.g.,

  (if #t 5 7)
  (if #f 5 7)

* lambda functions; e.g.,

  ((lambda (x) (* x 2)) 7)

* comparison operators (=, <, >, <=, >=)

WHAT DOESN'T WORK

* strings are accepted as atoms and can be defined as variables, but there
  are as yet very few string operations
* ...and anything else not listed under "WHAT WORKS" :-)

NOTES

* mathematical operations work in the following way:
  * a math operation with all integer operands will return an integer
  * a math operation with all float operands will return a float
  * a math operation with both integer and float operands will return a float
    * integer operations are used as long as integers are seen in the input;
      as soon as a float is seen, it's all floating-point operations from that
      point regardless of the type of subsequent operands
  * on subtraction: (- a b c d e) is the same as (- a (+ b c d e))
  * on multiplication: (/ a b c d e) is the same as (/ a (* b c d e))
