(define-module (xchat-guile plugin-system)
	       #:export (xchat:load-plugin
			 xchat:unload-plugin
			 xchat:unload-all-plugins))

(use-modules (xchat-guile main))

(define plugins '())
(define plugins-lock (make-mutex))

(define (register-plugin plugin)
  (let* ((filename
	   (basename 
	     (variable-ref (module-variable plugin 'xchat:plugin-file))))
	 (name 
	   (variable-ref (module-variable plugin 'xchat:plugin-name)))
	 (version
	   (variable-ref (module-variable plugin 'xchat:plugin-version)))
	 (description
	   (variable-ref (module-variable plugin 'xchat:plugin-description)))
	 (handle 
	   (xchat:plugin-gui-add filename name description version)))
    (set! plugins 
      (cons (list name filename version description plugin handle) plugins))))

;; This is the function that will be called each time user requests
;; to load a plugin
(define (xchat:load-plugin plugin-file)
  (let ((plugin (make-module)))
    (set-module-uses! plugin
		      (append (list (resolve-module '(guile-user))
				    (resolve-module '(srfi srfi-1))
				    (current-module))
			      (module-uses plugin)))
    (module-define! plugin 'xchat:plugin-file plugin-file)
    (eval '(load xchat:plugin-file) plugin)
    (eval '(xchat:plugin-init) plugin)
    (lock-mutex plugins-lock)
    (register-plugin plugin)
    (unlock-mutex plugins-lock)))

(define (unregister-plugin plugin-entry)
  (eval '(xchat:plugin-deinit) (list-ref plugin-entry 4))
  (xchat:plugin-gui-remove (list-ref plugin-entry 5))
  (set! plugins (delete plugin-entry plugins)))

(define (assocn key alist n)
  (cond ((null? alist) #f)
	((equal? key (list-ref (car alist) n)) (car alist))
	(#t (assocn key (cdr alist) n))))

(define (find-plugin plugin-name)
  (or (assocn plugin-name plugins 0)
      (assocn plugin-name plugins 1)))

;; This is the function that will be called each time user requests
;; to unload a plugin
(define (xchat:unload-plugin plugin-name)
  (let ((found #f))
    (lock-mutex plugins-lock)
    (let ((plugin (find-plugin plugin-name)))
      (if (not plugin)
	(set! found #f)
	(begin 
	  (unregister-plugin plugin)
	  (set! found #t))))
    (unlock-mutex plugins-lock)
    found))

(define (xchat:unload-all-plugins)
  (lock-mutex plugins-lock)
  (for-each unregister-plugin plugins)
  (unlock-mutex plugins-lock))

