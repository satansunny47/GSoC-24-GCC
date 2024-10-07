// To get the all exception types from a resx stmt
static bool
extract_types_for_resx (basic_block bb, vec<tree> *ret_vector)
{
  edge e;
  edge_iterator ei;

  // Iterate over edges to walk up the basic blocks
  FOR_EACH_EDGE (e, ei, bb->preds)
  {
    // Get the last stmt of the basic block as it is an EH stmt
    bb = e->src;
    gimple_stmt_iterator gsi = gsi_last_bb (bb);
    gimple *last_stmt = gsi_stmt (gsi);

    if (bb->aux)
      continue;
    bb->aux = (void *)1;

    if (last_stmt && (e->flags & EDGE_EH))
      {
        if (gimple_code (last_stmt) == GIMPLE_CALL)
          {
            // check if its a throw
            if (!extract_types_for_call (as_a<gcall *> (last_stmt),
                                         ret_vector))
              return false;
            continue;
          }
        else if (gimple_code (last_stmt) == GIMPLE_RESX)
          {
            // Recursively processing resx
            // FIXME: to get this linear, we should cache results.
            if (!extract_types_for_resx (last_stmt, ret_vector))
              return false;
            continue;
          }
      }
    /* FIXME: remove recursion here, so we do not run out of stack.  */
    else if (!extract_types_for_resx (e->src, ret_vector))
      return false;
  }
  return true;
}

// To get the all exception types from a resx stmt
bool
extract_types_for_resx (gimple *resx_stmt, vec<tree> *ret_vector)
{
  basic_block bb = gimple_bb (resx_stmt);
  bool ret = extract_types_for_resx (bb, ret_vector);
  /* FIXME: this is non-linear.  */
  clear_aux_for_blocks ();
  return ret;
}