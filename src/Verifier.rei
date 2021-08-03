include
  Kit.S with
    type authenticated_computation('a) =
      Kit.proof => [ | `Ok(Kit.proof, 'a) | `ProofFailure] and
    type auth('a) = string;
