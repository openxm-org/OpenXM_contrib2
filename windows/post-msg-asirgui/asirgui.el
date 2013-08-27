;; $OpenXM$
(defcustom asir-exec-path '("~/Desktop/asir/bin" "c:/Program Files/asir/bin" "c:/Program Files (x64)/asir/bin" "c:/asir/bin")
  "search path for asir binary")

(defun asir-executable-find (command) 
  "Search for command in asir-exec-path and exec-path"
  (let ((exec-path (append asir-exec-path exec-path)))
	(executable-find command)))

(defun execute-current-buffer-on-asir ()
  "Execute the current buffer on asir"
  (interactive)
  (let ((exec-path (append asir-exec-path exec-path)))
	(start-process "asir-proc-cmdasir" nil "cmdasir" (buffer-file-name))))

(defun start-asirgui ()
  "Execute the current buffer on asir"
  (interactive)
  (let ((exec-path (append asir-exec-path exec-path)))
	(start-process "asir-proc-asirgui" nil "asirgui"))) 

(defun execute-region-on-asir () 
  "Execute the region on asir"
  (interactive)
  (save-excursion
	(if (region-active-p)
		(let ((exec-path (append asir-exec-path exec-path))
			  (temp-file (make-temp-file (format "%s/asir-temp" (getenv "TEMP")))))
		  (write-region (region-beginning) (region-end) temp-file)
		  (start-process "asir-proc-cmdasir" nil "cmdasir" temp-file)))))

(define-key-after global-map [menu-bar asirgui]
  (cons "AsirGUI" (make-sparse-keymap "asirgui..."))
  'tools )

(define-key global-map [menu-bar asirgui execute-region]
  '("Execute the region on asir" . execute-region-on-asir))

(define-key global-map [menu-bar asirgui execute-buffer]
  '("Execute the current buffer on asir" . execute-current-buffer-on-asir))

(define-key global-map [menu-bar asirgui start-asirgui]
  '("Start asirgui" . start-asirgui))

