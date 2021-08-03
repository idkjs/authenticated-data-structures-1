module type S = {
  /* Authenticated data. */
  type auth('a);

  /* The authenticated computation monad. */
  type authenticated_computation('a);

  /* Monad structure */
  let return: 'a => authenticated_computation('a);

  let (>>=):
    (authenticated_computation('a), 'a => authenticated_computation('b)) =>
    authenticated_computation('b);

  /* Authenticatable data types */
  module Authenticatable: {
    type evidence('a);
    let auth: evidence(auth('a));
    let pair: (evidence('a), evidence('b)) => evidence(('a, 'b));
    let sum:
      (evidence('a), evidence('b)) =>
      evidence([ | `left('a) | `right('b)]);
    let string: evidence(string);
    let int: evidence(int);
    let unit: evidence(unit);
  };

  /* auth/unauth wrapping */
  let auth: (Authenticatable.evidence('a), 'a) => auth('a);

  let unauth:
    (Authenticatable.evidence('a), auth('a)) =>
    authenticated_computation('a);
};

type proof = list(Ezjsonm.value);

let hash_json = {
  let h = Cryptokit.Hash.sha1();
  json => Cryptokit.hash_string(h, Ezjsonm.to_string(`A([json])));
};
