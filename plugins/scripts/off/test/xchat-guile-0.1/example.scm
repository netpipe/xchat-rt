(define xchat:plugin-name "guile-example")
(define xchat:plugin-description "This is a demo XChat Guile plugin")
(define xchat:plugin-version "0.1")

(define timer-id #f)

(define (xchat:plugin-init)
  ; This code deliberately makes use of closures to demonstrate why this
  ; procedure does not need a 'user-data' argument in scheme as needed in
  ; other languages
  (set! timer-id 
	(xchat:hook-timer 1000 
			  (let ((clock 0))
			    (lambda ()
			      (set! clock (+ clock 1))
			      (display "seconds passed = ")
			      (display clock)
			      (newline)))))
  (display "Plugin Initialized")
  (newline))

(define (xchat:plugin-deinit)
  (xchat:unhook timer-id)
  (display "Plugin De-initialized")
  (newline))

