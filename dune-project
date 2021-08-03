(lang dune 2.9)

(name authentikit)

(generate_opam_files true)

(license MIT)

(maintainers "idkjs")

(package
 (name authentikit)
 (synopsis "authentikit")
 (description "authentikit")
 (depends
  ;; General system dependencies
  (dune
   (>= 2.2))
  (ocaml
   (>= 4.12.0))
  (cryptokit
   (>= 1.16.1))
  (ezjsonm
   (>= 1.2.0))
  (reason
   (and
    (>= 3.6.0)
    (< 4.0.0)))
  ;; Dev dependencies
  (ocaml-lsp-server :dev)))
