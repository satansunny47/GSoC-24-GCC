// To get the types being thrown outside of a function
bool
extract_fun_resx_types (function *fun, vec<tree> *ret_vector)
{
  basic_block bb;
  gimple_stmt_iterator gsi;
  hash_set<tree> types;

  FOR_EACH_BB_FN (bb, fun)
  {
    bb->aux = (void *)1;
    gsi = gsi_last_bb (bb);
    gimple *stmt = gsi_stmt (gsi);
    auto_vec<tree> resx_types;

    if (!stmt || !stmt_can_throw_external (fun, stmt))
      continue;

    if (gimple_code (stmt) == GIMPLE_RESX)
      {
        if (!extract_types_for_resx (stmt, &resx_types))
          return false;
      }

    else if (gimple_code (stmt) == GIMPLE_CALL)
      {
        if (!extract_types_for_call (as_a<gcall *> (stmt), &resx_types))
          return false;
      }

    for (unsigned i = 0; i < resx_types.length (); ++i)
      {
        tree type = resx_types[i];
        types.add (type);
      }
  }

  for (auto it = types.begin (); it != types.end (); ++it)
    {
      ret_vector->safe_push (*it);
    }

  return true;
}