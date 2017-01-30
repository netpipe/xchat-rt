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

