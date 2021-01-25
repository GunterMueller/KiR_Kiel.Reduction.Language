;; general settings ******************************************
(line-number-mode 1)
(column-number-mode 1)
(setq visible-bell t)

;; (custom-set-variables
;;  '(scroll-bar-mode (quote right)))
;; (custom-set-faces)

(setq auto-mode-alist '(("\\.c$" . c-mode) ("\\.sac$" . c-mode) ("\\.cpp$" . c++-mode) ("\\.h$" . c++-mode)))

;; load libraries/programs ******************************************
(load "paren")

;; function declarations ******************************************
(defun my-c-mode-hook ()
  (interactive)
  (font-lock-mode 1)
  (setq comment-multi-line t)
  (setq indent-tabs-mode nil)
  )

(defun my-delete-line ()
  (interactive)
  (beginning-of-line)
  (kill-line)
  (delete-char 1)
  (indent-for-tab-command)
  )

(defun fun ()
  "insert function header, register K is used"
  (interactive)
  (beginning-of-line)
  ;store declaration to register K and kill it.
  (let ((beg (point))) 
       (end-of-line) 
       (copy-to-register 'K (point) beg)
       (forward-line 1)
       (beginning-of-line)
       (delete-region beg (point)))
  
  ;insert headerfile and set function name
  (insert-file "~/sac/BASE/sac2c/doc/fun")
  (forward-line 3)
  (end-of-line)
  (insert "   ")
  (insert-register 'K)
  (forward-line 6)
  (newline)
  (insert-register 'K)

  ;insert function name into DBUG_ENTER
  (forward-word 1) (forward-char 1)
  (let ((beg (point))) 
       (forward-word 1)
       (copy-to-register 'K beg (point)))
  (forward-line 2)
  (forward-char 15)
  (insert-register 'K)
  (forward-line -6)
  (end-of-line)
  (insert "   ")
  )

;; hooks ******************************************
(add-hook 'c-mode-hook '(lambda () (my-c-mode-hook)))
(add-hook 'c++-mode-hook '(lambda () (my-c-mode-hook)))
(add-hook 'text-mode-hook 'turn-on-auto-fill)

;; c-mode-settings ******************************************
(setq c-auto-newline nil)
(setq font-lock-maximum-decoration t)

;; keybindigs ******************************************
(global-set-key "\C-c\C-o" 'occur)
(global-set-key "\C-c\C-r" 'eval-region)
(global-set-key "\C-c\C-d" 'eval-defun)
(global-set-key "\C-z"     'my-delete-line)
(global-set-key "\C-t"     'kill-word)
(global-set-key "\C-k"     'kill-this-buffer)
(global-set-key "\C-f"     'search-forward)
(global-set-key "\C-l"     'repeat-search-forward)
(global-set-key "\C-a"     'query-replace)
(global-set-key "\C-r"     'recenter)
(global-set-key "\C-c\C-o" 'revert-buffer)
(global-set-key "\C-o"     'find-file)

(global-set-key [home]     'beginning-of-line)
(global-set-key [end]      'end-of-line)
(global-set-key [backspace]   'backward-delete-char)
(global-set-key [delete]   'delete-char)

;; Options Menu Settings
;; =====================
(cond
 ((and (string-match "XEmacs" emacs-version)
       (boundp 'emacs-major-version)
       (or (and
            (= emacs-major-version 19)
            (>= emacs-minor-version 14))
           (= emacs-major-version 20))
       (fboundp 'load-options-file))
  (load-options-file "/home/dkr/.xemacs-options")))
;; ============================
;; End of Options Menu Settings
