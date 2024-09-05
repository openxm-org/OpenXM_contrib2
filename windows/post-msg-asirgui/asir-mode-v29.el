;; -*- mode: emacs-lisp  -*-
;;
;; asir-mode.el -- asir mode
;;
;; $OpenXM$

;; This program is free software: you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation, either version 3 of the License, or
;; (at your option) any later version.

;; 1. Install 
;;
;; **(for Windows) Write the following configuration to your .emacs file.
;;
;; (setq load-path (append load-path '((concat (getenv "ASIR_ROOTDIR") "/share/editor"))))
;; (setq auto-mode-alist (cons '("\\.rr$" . asir-mode) auto-mode-alist))
;; (autoload 'asir-mode "asir-mode" "Asir mode" t)
;;
;; **(for unix) Copy this file to your emacs site-lisp folder and 
;;              write the following configuration to your .emacs file.
;;
;; (setq auto-mode-alist (cons '("\\.rr$" . asir-mode) auto-mode-alist))
;; (autoload 'asir-mode "asir-mode" "Asir mode" t)
;;
;; Please run byte-compile for speed up.
;;
;; 2. Use
;;
;; If you open Risa/Asir source file (*.rr) by emacs, then asir-mode starts up automatically.
;; The following key binding can be used:
;; C-c s     Asir starts up in another window.
;; C-c t     Asir terminates.
;; C-c a     Abort current calculation.
;; C-c l     The current buffer is loaded to Asir as a file.
;; C-c r     Selected region is loaded to Asir as a file.
;; C-c p     Selected region is pasted to Asir.

(require 'shell)
;(require 'cl)

;;;; AsirGUI for Windows
(defvar asir-exec-path '("~/Desktop/asir/bin" "c:/Program Files/asir/bin" "c:/Program Files (x64)/asir/bin" "c:/asir/bin")
  "Default search path for asir binary in Windows")

(defun asir-effective-exec-path ()
  "Search path for command"
  (let* ((dir (getenv "ASIR_ROOTDIR"))
         (path (append asir-exec-path exec-path)))
    (if dir (cons (concat dir "/bin") path) path)))

(defun asir-executable-find (command)
  "Search for command"
  (let* ((exec-path (asir-effective-exec-path)))
    (executable-find command)))

;;;; Asir for UNIX
(defvar asir-cmd-buffer-name "*asir-cmd*")

(defun asir-cmd-load (filename)
  "Send `load' command to running asir process"
  (if (eq system-type 'windows-nt)
      (let ((exec-path (asir-effective-exec-path)))
        (start-process "asir-proc-cmdasir" nil "cmdasir" filename))
    (save-excursion
      (if (get-buffer asir-cmd-buffer-name)
          (progn
            (set-buffer asir-cmd-buffer-name)
            (goto-char (point-max))
            (insert (format "load(\"%s\");" filename))
            (comint-send-input))))))

(defun asir-start ()
  "Start asir process"
  (interactive)
  (if (eq system-type 'windows-nt)
    ;; for Windows
      (let ((exec-path (asir-effective-exec-path)))
        (start-process "asir-proc-asirgui" nil "asirgui"))
    ;; for UNIX
    (save-excursion
      (if (not (get-buffer asir-cmd-buffer-name))
          (let ((current-frame (selected-frame)))
            (if window-system 
                (progn 
                  (select-frame (make-frame))
                  (shell (get-buffer-create asir-cmd-buffer-name)) ;; Switch to new buffer automatically
                  (delete-other-windows))
              (if (>= emacs-major-version 24)
                  (progn 
                    (split-window)
                    (other-window -1)))
              (shell (get-buffer-create asir-cmd-buffer-name)))
            (sleep-for 1)
            (goto-char (point-max))
            (insert "asir -nofep")
            (comint-send-input)
            (select-frame current-frame))))))

(defun asir-terminate ()
  "Terminate asir process"
  (interactive)
  (if (eq system-type 'windows-nt)
    ;; for Windows
      (let ((exec-path (asir-effective-exec-path)))
        (start-process "asir-proc-cmdasir" nil "cmdasir" "--quit"))
    ;; for UNIX
    (if (get-buffer asir-cmd-buffer-name)
        (if (not window-system)
            (let ((asir-cmd-window (get-buffer-window asir-cmd-buffer-name)))
              (and (kill-buffer asir-cmd-buffer-name)
                   (or (not asir-cmd-window) (delete-window asir-cmd-window))))
          (let ((asir-cmd-frame (window-frame (get-buffer-window asir-cmd-buffer-name 0))))
            (and (kill-buffer asir-cmd-buffer-name)
                 (delete-frame asir-cmd-frame)))))))

(defun asir-execute-current-buffer ()
  "Execute current buffer on asir"
  (interactive)
  (let ((exec-path (asir-effective-exec-path)))
    (asir-cmd-load (buffer-file-name))))

(defun asir-execute-region ()
  "Execute region on asir"
  (interactive)
  (if mark-active
      (save-excursion
        (let ((temp-file (make-temp-file (format "%s/cmdasir-" (or (getenv "TEMP") "/var/tmp"))))
              (temp-buffer (generate-new-buffer " *asir-temp*")))
          (write-region (region-beginning) (region-end) temp-file)
          (set-buffer temp-buffer)
          (insert " end$")
          (write-region (point-min) (point-max) temp-file t) ;; append
          (kill-buffer temp-buffer)
          (asir-cmd-load temp-file)))))

(defun asir-paste-region ()
  "Paste region to asir"
  (interactive)
  (if mark-active
      (if (eq system-type 'windows-nt)
          (let ((temp-file (make-temp-file (format "%s/cmdasir-" (getenv "TEMP"))))
                (exec-path (asir-effective-exec-path)))
            (write-region (region-beginning) (region-end) temp-file)
            (start-process "asir-proc-cmdasir" nil "cmdasir" "--paste-contents" temp-file))
        (save-excursion
          (let ((buffer (current-buffer))
                (start (region-beginning))
                (end (region-end)))
            (set-buffer asir-cmd-buffer-name)
            (goto-char (point-max))
            (insert-buffer-substring buffer start end)
            (comint-send-input))))))

(defun asir-abort ()
  "Abort calculation on asir"
  (interactive)
  (if (eq system-type 'windows-nt)
    ;; for Windows
      (let ((exec-path (asir-effective-exec-path)))
        (start-process "asir-proc-cmdasir" nil "cmdasir" "--abort"))
    ;; for UNIX
    (save-excursion
      (if (get-buffer asir-cmd-buffer-name)
          (progn
            (set-buffer asir-cmd-buffer-name)
            (comint-kill-input)
            (comint-interrupt-subjob)
            (goto-char (point-max))
            (insert "t\ny")
            (comint-send-input)
            )))))

;;;; Extension for CC-mode.

(require 'cc-mode)

(eval-when-compile
  (require 'cc-langs)
  (require 'cc-engine)
  (require 'cc-fonts)
)

(eval-and-compile
  ;; Make our mode known to the language constant system.  Use Java
  ;; mode as the fallback for the constants we don't change here.
  ;; This needs to be done also at compile time since the language
  ;; constants are evaluated then.
  (c-add-language 'asir-mode 'c++-mode))

(c-lang-defconst c-stmt-delim-chars asir "^;${}?:")
(c-lang-defconst c-stmt-delim-chars-with-comma asir "^;$,{}?:")
(c-lang-defconst c-other-op-syntax-tokens asir (cons "$" (c-lang-const c-other-op-syntax-tokens c)))
(c-lang-defconst c-identifier-syntax-modifications asir (remove '(?$ . "w") (c-lang-const c-identifier-syntax-modifications c)))
(c-lang-defconst c-symbol-chars asir (concat c-alnum "_"))
(c-lang-defconst c-primitive-type-kwds asir '("def" "extern" "static" "localf" "function"))
(c-lang-defconst c-primitive-type-prefix-kwds asir nil)
(c-lang-defconst c-type-list-kwds asir nil)
(c-lang-defconst c-class-decl-kwds asir '("module"))
(c-lang-defconst c-othe-decl-kwds  asir '("endmodule" "end"))
;; (c-lang-defconst c-type-modifier-kwds asir nil)
(c-lang-defconst c-modifier-kwds asir nil)

(c-lang-defconst c-mode-menu 
  asir
  (append (c-lang-const c-mode-menu c)
		  '("----" 
			["Start Asir" asir-start t]
			["Terminate Asir" asir-terminate t]
			["Abort calcuration on Asir" asir-abort t]
			["Execute Current Buffer on Asir" asir-execute-current-buffer (buffer-file-name)]
			["Execute Region on Asir" asir-execute-region mark-active]
			["Paste Region to Asir" asir-paste-region mark-active]
			)))

(defvar asir-font-lock-extra-types nil
  "*List of extra types (aside from the type keywords) to recognize in asir mode.
Each list item should be a regexp matching a single identifier.")

(defconst asir-font-lock-keywords-1 (c-lang-const c-matchers-1 asir)
  "Minimal highlighting for asir mode.")

(defconst asir-font-lock-keywords-2 (c-lang-const c-matchers-2 asir)
  "Fast normal highlighting for asir mode.")

(defconst asir-font-lock-keywords-3 (c-lang-const c-matchers-3 asir)
  "Accurate normal highlighting for asir mode.")

(defvar asir-font-lock-keywords asir-font-lock-keywords-3
  "Default expressions to highlight in asir mode.")

(defvar asir-mode-syntax-table nil
  "Syntax table used in asir-mode buffers.")
(or asir-mode-syntax-table
    (setq asir-mode-syntax-table
	  (funcall (c-lang-const c-make-mode-syntax-table asir))))

(defvar asir-mode-abbrev-table nil
  "Abbreviation table used in asir-mode buffers.")

(defvar asir-mode-map (let ((map (c-make-inherited-keymap)))
		      ;; Add bindings which are only useful for asir
		      map)
  "Keymap used in asir-mode buffers.")

;; Key binding for asir-mode
(define-key asir-mode-map (kbd "C-c s") 'asir-start)
(define-key asir-mode-map (kbd "C-c t") 'asir-terminate)
(define-key asir-mode-map (kbd "C-c a") 'asir-abort)
(define-key asir-mode-map (kbd "C-c l") 'asir-execute-current-buffer)
(define-key asir-mode-map (kbd "C-c r") 'asir-execute-region)
(define-key asir-mode-map (kbd "C-c p") 'asir-paste-region)

(easy-menu-define asir-menu asir-mode-map "asir Mode Commands"
		  ;; Can use `asir' as the language for `c-mode-menu'
		  ;; since its definition covers any language.  In
		  ;; this case the language is used to adapt to the
		  ;; nonexistence of a cpp pass and thus removing some
		  ;; irrelevant menu alternatives.
		  (cons "Asir" (c-lang-const c-mode-menu asir)))

(defun asir-mode ()
  "Major mode for editing asir code.
This is a simple example of a separate mode derived from CC Mode to
support a language with syntax similar to C/C++/ObjC/Java/IDL/Pike.

The hook `c-mode-common-hook' is run with no args at mode
initialization, then `asir-mode-hook'.

Key bindings:
\\{asir-mode-map}"
  (interactive)
  (kill-all-local-variables)
  (c-initialize-cc-mode t)
  (set-syntax-table asir-mode-syntax-table)
  (setq major-mode 'asir-mode
	mode-name "asir"
	local-abbrev-table asir-mode-abbrev-table
	abbrev-mode t)
  (use-local-map asir-mode-map)
  ;; `c-init-language-vars' is a macro that is expanded at compile
  ;; time to a large `setq' with all the language variables and their
  ;; customized values for our language.
  (c-init-language-vars asir-mode)
  ;; `c-common-init' initializes most of the components of a CC Mode
  ;; buffer, including setup of the mode menu, font-lock, etc.
  ;; There's also a lower level routine `c-basic-common-init' that
  ;; only makes the necessary initialization to get the syntactic
  ;; analysis and similar things working.
  (c-common-init 'asir-mode)
; (easy-menu-add asir-menu)
  (run-hooks 'c-mode-common-hook)
  (run-hooks 'asir-mode-hook)
  (c-update-modeline)
)

;; Meadow 3 does not have `c-brace-anchor-point'
;; This function was copied from cc-engine.el of Emacs 23.4
(if (and (featurep 'meadow) (not (fboundp 'c-brace-anchor-point)))
    (defun c-brace-anchor-point (bracepos)
      ;; BRACEPOS is the position of a brace in a construct like "namespace
      ;; Bar {".  Return the anchor point in this construct; this is the
      ;; earliest symbol on the brace's line which isn't earlier than
      ;; "namespace".
      ;;
      ;; Currently (2007-08-17), "like namespace" means "matches
      ;; c-other-block-decl-kwds".  It doesn't work with "class" or "struct"
      ;; or anything like that.
      (save-excursion
        (let ((boi (c-point 'boi bracepos)))
          (goto-char bracepos)
          (while (and (> (point) boi)
                      (not (looking-at c-other-decl-block-key)))
            (c-backward-token-2))
          (if (> (point) boi) (point) boi))))
  )

(provide 'asir-mode)
