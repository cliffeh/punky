(define null? (lambda (l) (= l ()))) ;expect: null?

(define cddr (lambda (l) (cdr (cdr l)))) ;expect: cddr

(define is-sorted? ; #t if list is sorted; otherwise #f
  (lambda (l)
    (if (null? (cdr l)) #t ; single-element lists are always sorted
      ; else
      (and 
        (< (car l) (car (cdr l)))
        (is-sorted? (cdr l))
      ))
  )
) ;expect: is-sorted?

(define less-than-list
  (lambda (a l)
    (if (null? l) ()
      ; else
      (if (< (car l) a)
	      (cons (car l) (less-than-list a (cdr l)))
        ; else
	      (less-than-list a (cdr l))
	    ))
  )
) ;expect: less-than-list

(define greater-equal-list
  (lambda (a l)
    (if (null? l) ()
      ; else
      (if (>= (car l) a)
	      (cons (car l) (greater-equal-list a (cdr l)))
	      (greater-equal-list a (cdr l))
	    ))
  )
) ;expect: greater-equal-list
	

(defun sort (l)
  (if (null? l) ()
    ; else
    (append
      (sort (less-than-list (car l) (cdr l)))                    ; sorted list of everything <  (car l)
      (cons (car l) (sort (greater-equal-list (car l) (cdr l)))) ; sorted list of everything >= (car l)
    )
  )
) ;expect: sort

(sort '(1))                       ;expect: (1)
(sort '(2 1))                     ;expect: (1 2)
(sort '(1 2 3 4 5 6 7 8 9))       ;expect: (1 2 3 4 5 6 7 8 9)
(sort '(2 7 1 9 5 3 4 8 6))       ;expect: (1 2 3 4 5 6 7 8 9)
(is-sorted? '(2 7 1 9 5 3 4 8 6)) ;expect: #f
(is-sorted? '(1 2 3 4 5 6 7 8 9)) ;expect: #t
