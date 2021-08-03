module type Merkle =
  (A: Authentikit.Kit.S) =>
   {
    open A;

    type path = list([ | `L | `R]);
    type tree = A.auth([ | `left(string) | `right(tree, tree)]);

    let make_leaf: string => tree;
    let make_branch: (tree, tree) => tree;
    let retrieve: (path, tree) => authenticated_computation(option(string));
    let update:
      (path, string, tree) => authenticated_computation(option(tree));
  };

module Merkle: Merkle =
  (A: Authentikit.Kit.S) => {
    open A;

    type path = list([ | `L | `R]);
    type tree = A.auth([ | `left(string) | `right(tree, tree)]);

    let tree:
      Authenticatable.evidence([ | `left(string) | `right(tree, tree)]) = (
      Authenticatable.(sum(string, pair(auth, auth))):
        Authenticatable.evidence([ | `left(string) | `right(tree, tree)])
    );

    let make_leaf = s => auth(tree, `left(s));

    let make_branch = (l, r) => auth(tree, `right((l, r)));

    let rec retrieve = (path, t) =>
      unauth(tree, t)
      >>= (
        t =>
          switch (path, t) {
          | ([], `left(s)) => return(Some(s))
          | ([`L, ...path], `right(l, r)) => retrieve(path, l)
          | ([`R, ...path], `right(l, r)) => retrieve(path, r)
          | (_, _) => return(None)
          }
      );

    let rec update = (path, v, t) =>
      unauth(tree, t)
      >>= (
        t =>
          switch (path, t) {
          | ([], `left(_)) => return(Some(make_leaf(v)))
          | ([`L, ...path], `right(l, r)) =>
            update(path, v, l)
            >>= (
              fun
              | None => return(None)
              | Some(l') => return(Some(make_branch(l', r)))
            )
          | ([`R, ...path], `right(l, r)) =>
            update(path, v, r)
            >>= (
              fun
              | None => return(None)
              | Some(r') => return(Some(make_branch(l, r')))
            )
          | _ => return(None)
          }
      );
  };

module Prover = Merkle(Authentikit.Prover);
module Verifier = Merkle(Authentikit.Verifier);
