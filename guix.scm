;; guix.scm
(use-modules
  (guix)
  (guix git-download)
  (guix packages)
  (guix gexp)
  (guix download)
  (guix build-system cmake)
  (gnu packages)
  (gnu packages bash)
  (gnu packages base)
  (gnu packages cmake)
  (gnu packages llvm)
  (gnu packages ninja)
  (gnu packages python)
  (gnu packages python-build)
  (gnu packages python-xyz)
  (gnu packages check)
  (gnu packages libffi)
  (gnu packages pkg-config))
(define vcs-file?
  ;; Return true if the given file is under version control.
  (or (git-predicate (current-source-directory))
      (const #t)))                                ;not in a Git checkout

(package
  (name "slang")
  (version "0.1.0")

  (source (local-file "." "guile-checkout"
                      #:recursive? #t
                      #:select? vcs-file?))
  (build-system cmake-build-system)

  (arguments
   (list
    #:build-type "Release"
    #:tests? #f                  ;; disable tests

    #:configure-flags
    #~(list
       (string-append
        "-DPython3_EXECUTABLE="
        (search-input-file %build-inputs "/bin/python3"))

       (string-append
        "-DLLVM_EXTERNAL_LIT="
        (search-input-file %build-inputs "/bin/.lit-real")))

    #:generator "Ninja"))

  (native-inputs
   (list
    bash
    coreutils
    which
    cmake
    ninja
    clang-19
    clang-toolchain-19
    binutils
    python
    python-lit
    python-nanobind
    pybind11
    cmakelang))

  (inputs
   (list
    mlir-19
    libffi))

  (synopsis "slang compiler project")
  (description
   "Build recipe for the slang compiler project using MLIR, nanobind,
pybind11, clang, and Ninja.")
  (home-page "https://example.org/slang")
  (license #f))
