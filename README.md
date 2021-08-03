# Generic Authenticated Data Structures

An implementation in OCaml of Miller et al's [Authenticated Data
Structures,
Generically](http://www.cs.umd.edu/~mwh/papers/gpads.pdf). See the
blog post [Authenticated Data Structures, as a Library, for
Free!](https://bentnib.org/posts/2016-04-12-authenticated-data-structures-as-a-library.html) for more information.

To compile it, you'll need OCaml (tested with version 4.05.0), `opam`,
`dune`. You'll need to install the
prerequisites `ezjsonm` and `cryptokit`:

    opam install ezjsonm cryptokit

To play with it interactively, we need to install the nice OCaml
REPL `utop`:

    opam install utop

Then to compile the example, Merkle trees, and load it into `utop` do:

    dune utop test

Example interaction:

```ocaml
(* create a tree *)
# let tree =
    Merkle.Prover.(make_branch
                     (make_branch (make_leaf "a") (make_leaf "b"))
                     (make_branch (make_leaf "c") (make_leaf "d")));;
val tree : Merkle.Prover.tree = <abstr>

(* get the hash code of the root *)
# let code = Authentikit.Prover.get_hash tree;;
val code : string = ".z\129w\199J\224\\\254\220\bo\246W\158\243S\029\177\190"

(* run a query on the server side, get a proof and a result *)
# let proof, result = Merkle.Prover.retrieve [`L;`L] tree;;
val proof : Authentikit.Kit.proof = [<abstr>; <abstr>; <abstr>]
val result : string option = Some "a"

(* verify the proof on the client side *)
# Merkle.Verifier.retrieve [`L;`L] code proof;;
- : [ `Ok of Authentikit.Kit.proof * string option | `ProofFailure ] = `Ok ([], Some "a")

(* Let's make another tree and try to trick the verifier *)
# let other_tree =
    Merkle.Prover.(make_branch
                     (make_branch (make_leaf "A") (make_leaf "B"))
                     (make_branch (make_leaf "C") (make_leaf "D")));;
val other_tree : Merkle.Prover.tree = <abstr>

(* Run a query on this tree *)
# let proof, result = Merkle.Prover.retrieve [`L;`L] other_tree;;
val proof : Authentikit.Kit.proof = [<abstr>; <abstr>; <abstr>]
val result : string option = Some "A"

(* Now verifying this proof against the code for the original tree fails: *)
# Merkle.Verifier.retrieve [`L;`L] code proof;;
- : [ `Ok of Authentikit.Kit.proof * string option | `ProofFailure ] = `ProofFailure
```
```reason
// create a tree
utop # #require "rtop";;
Reason # let tree =
  Merkle.Prover.(
    make_branch(
      make_branch(make_leaf("a"), make_leaf("b")),
      make_branch(make_leaf("c"), make_leaf("d")),
    )
  );
let tree: Merkle.Prover.tree = <abstr>;

/* get the hash code of the root */
let code = Authentikit.Prover.get_hash(tree);
let code: string = ".zwÇJà\\þÜ\boöWS\029±¾";

/* run a query on the server side, get a proof and a result */
Reason # let (proof, result) = Merkle.Prover.retrieve([`L, `L], tree);
let proof: Authentikit.Kit.proof =
  [`A([`String("right"),
       `A([`String("?úm&,û\031\rüQJ\001|d}ò\016l"),
           `String("i?Bæpø\0000pà\018\023ÛÁ5")])]),
   `A([`String("right"),
       `A([`String("X\005\028½1LàöàåÉ\018o\b»£ð "),
           `String("ßçÂæ2=>ø\014;Éþ")])]),
   `A([`String("left"), `String("a")])];
let result: option(string) = Some("a");

/* verify the proof on the client side */
Reason # Merkle.Verifier.retrieve([`L, `L], code, proof);
- : [ `Ok(Authentikit.Kit.proof, option(string))
    | `ProofFailure ]
= `Ok(([], Some("a")))

/* Let's make another tree and try to trick the verifier */
Reason # let other_tree =
  Merkle.Prover.(
    make_branch(
      make_branch(make_leaf("A"), make_leaf("B")),
      make_branch(make_leaf("C"), make_leaf("D")),
    )
  );let (proof, result) = Merkle.Prover.retrieve([`L, `L], other_tree);
let other_tree: Merkle.Prover.tree = <abstr>;

/* Run a query on this tree */
Reason # let (proof, result) = Merkle.Prover.retrieve([`L, `L], other_tree);
let proof: Authentikit.Kit.proof =
  [`A([`String("right"),
       `A([`String("QÙG\000öî!øÔ¸³>\017zW\0182("),
           `String("0ïî\002b4¬\002@-=g³Å\022|")])]),
   `A([`String("right"),
       `A([`String("êNêQS¥Ä\0038
j\018uY
\030\005"),
           `String("ã2|ßMæñ
                           §\029-\016Ý\005nxﾸ")])]),
   `A([`String("left"), `String("A")])];
let result: option(string) = Some("A");

/* Now verifying this proof against the code for the original tree fails: */
Merkle.Verifier.retrieve([`L, `L], code, proof);
Reason # Merkle.Verifier.retrieve([`L, `L], code, proof);
- : [ `Ok(Authentikit.Kit.proof, option(string))
    | `ProofFailure ]
= `ProofFailure
```


## Structure of the code

The code from the blog post has been split up. The `src` directory
holds the core library `Authentikit`. There are three modules:

- `Authentikit.Kit` contains the interface `S` and the definition of `proof`s and
  a hashing function used by provers and verifiers.

- `Authentikit.Prover` implements the prover / server side of the authenticated
  data structure implementation.

- `Authentikit.Verifier` implements the verifier / client side of the
  authenticated data structure implementation.

The `test` directory holds an example use of the library: Merkle
trees in `merkle.ml`.

## See Also

- [An implementation of the same idea in Haskell](https://github.com/adjoint-io/auth-adt)
- [Another implementation in Haskell, explicitly based on this one](https://github.com/ekmett/auth)
- [And another implementation in Haskell](https://github.com/trailofbits/indurative) which mentions this code as an inspiration.
