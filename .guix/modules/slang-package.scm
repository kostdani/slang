;; [[file:slang.org::*Project setup][Project setup:2]]
(define-module (guile-package)
  #:use-module (guix)
  #:use-module (guix packages)
  #:use-module (guix gexp)
  #:use-module (guix build-system cmake)
  #:use-module ((guix licenses) #:prefix license:)
  #:use-module (gnu packages llvm)
  #:use-module (gnu packages libffi))

(define-public slang
  (package
   (name "slang")
   (version "0.0.1")

   (source
    (local-file "." "slang-source"
		#:recursive? #t))

   (build-system cmake-build-system)

   (arguments
    (list
     #:build-type "Release"
     #:tests? #f
     ))

   (inputs
    (list
     llvm-19
     mlir-19
     libffi))

   (synopsis
    "Compiler frontend for S-Expression languages using MLIR")

   (description
    "slang is a compiler frontend for S-Expression based languages
built on top of MLIR and LLVM infrastructure.  It targets Lisp- and
Scheme-like languages and provides tooling for building extensible
compiler pipelines and language frontends.")

   (home-page
    "https://github.com/kostdani/slang")

   (license license:expat)))

slang
;; Project setup:2 ends here
