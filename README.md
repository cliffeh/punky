# punky

__a simple functional language interpreter__

## BUILDING

<code>make</code>

## RUNNING

<code>./punky</code>

## WHAT WORKS

* simple integer and floating-point arithmetic; e.g.,

  * <code>(+ 1 2)</code> => <code>3</code>
  * <code>(* 5 7)</code> => <code>35</code>
  * <code>(+ 4 (* 6 3) 5)</code> => <code>27</code>

* variable and function definition; e.g.,

  * <code>(define foo 27)</code>
  * <code>foo</code> => <code>27</code>
  * <code>(define double (lambda (x) (* x 2)))</code>
  * <code>(double 4)</code> => <code>8</code>
  * <code>(double (double 7))</code> => <code>28</code>

* quote, car, cdr, cons, list; e.g.,

  * <code>(car '(a b c d))</code> => <code>a</code>
  * <code>(list 'a 'b 'c 'd)</code> => <code>(a b c d)</code>
  * <code>(cons 1 '(2 3 4))</code> => <code>(1 2 3 4)</code>

* dot notation; e.g.,

  * <code>(1 . 2)</code>

* let; e.g.,

  * <code>(let ((x 10) (y 20)) (+ x y))</code> => <code>30</code>

* string operations: substr, strlen

  * <code>(substr "abcdefg" 2 3) => "cde"</code>
  * <code>(strlen "abcdefg") => 7</code>

* simple boolean operations; e.g.,

  * <code>(not #t)</code> => <code>#f</code>
  * <code>(and #f #t)</code> => <code>#f</code>
  * <code>(or #t #f)</code> => <code>#t</code>

* if statements; e.g.,

  * <code>(if #t 5 7)</code> => <code>5</code>
  * <code>(if #f 5 7)</code> => <code>7</code>

* lambda functions; e.g.,

  * <code>((lambda (x) (* x 2)) 7)</code>

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
