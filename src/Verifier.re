type auth('a) = string;

type authenticated_computation('a) =
  Kit.proof => [ | `Ok(Kit.proof, 'a) | `ProofFailure];

let return = (a, proof) => `Ok((proof, a));

let (>>=) = (c, f, prfs) =>
  switch (c(prfs)) {
  | `ProofFailure => `ProofFailure
  | `Ok(prfs', a) => f(a, prfs')
  };

module Authenticatable = {
  type evidence('a) = {
    serialise: 'a => Ezjsonm.value,
    deserialise: Ezjsonm.value => option('a),
  };

  let auth = {
    let serialise = h => `String(h)
    and deserialise =
      fun
      | `String(s) => Some(s)
      | _ => None;

    {serialise, deserialise};
  };

  let pair = (a_s, b_s) => {
    let serialise = ((a, b)) => `A([a_s.serialise(a), b_s.serialise(b)])
    and deserialise =
      fun
      | `A([x, y]) =>
        switch (a_s.deserialise(x), b_s.deserialise(y)) {
        | (Some(a), Some(b)) => Some((a, b))
        | _ => None
        }
      | _ => None;

    {serialise, deserialise};
  };

  let sum = (a_s, b_s) => {
    let serialise =
      fun
      | `left(a) => `A([`String("left"), a_s.serialise(a)])
      | `right(b) => `A([`String("right"), b_s.serialise(b)])
    and deserialise =
      fun
      | `A([`String("left"), x]) =>
        switch (a_s.deserialise(x)) {
        | Some(a) => Some(`left(a))
        | _ => None
        }
      | `A([`String("right"), y]) =>
        switch (b_s.deserialise(y)) {
        | Some(b) => Some(`right(b))
        | _ => None
        }
      | _ => None;

    {serialise, deserialise};
  };

  let string = {
    let serialise = s => `String(s)
    and deserialise =
      fun
      | `String(s) => Some(s)
      | _ => None;

    {serialise, deserialise};
  };

  let int = {
    let serialise = i => `String(string_of_int(i))
    and deserialise =
      fun
      | `String(i) =>
        try(Some(int_of_string(i))) {
        | Failure(_) => None
        }
      | _ => None;

    {serialise, deserialise};
  };

  let unit = {
    let serialise = () => `Null
    and deserialise =
      fun
      | `Null => Some()
      | _ => None;

    {serialise, deserialise};
  };
};

open Authenticatable;

let auth = (auth_evidence, a) => Kit.hash_json(auth_evidence.serialise(a));

let unauth = (auth_evidence, h) =>
  fun
  | [] => `ProofFailure
  | [p, ...ps] when Kit.hash_json(p) == h =>
    switch (auth_evidence.deserialise(p)) {
    | None => `ProofFailure
    | Some(a) => `Ok((ps, a))
    }
  | _ => `ProofFailure;
