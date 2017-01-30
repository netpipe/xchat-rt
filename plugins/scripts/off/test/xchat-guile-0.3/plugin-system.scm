;
; Copyright (C) 2009 Zeeshan Ali (Khattak) <zeeshanak@gnome.org>.
;
; This file is part of XChat-Guile.
;
; XChat-Guile is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
; XChat-Guile is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software Foundation,
; Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

(define-module (xchat-guile plugin-system)
	       #:export (xchat:load-plugin
			 xchat:unload-plugin
			 xchat:unload-all-plugins))

(use-modules (xchat-guile main))

(define plugins '())

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
    (register-plugin plugin)))

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
    (let ((plugin (find-plugin plugin-name)))
      (if (not plugin)
	(set! found #f)
	(begin 
	  (unregister-plugin plugin)
	  (set! found #t))))
    found))

(define (xchat:unload-all-plugins)
  (for-each unregister-plugin plugins))

