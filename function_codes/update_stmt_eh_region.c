/* Function to update landing pad and region in throw_stmt_table for a given
statement */
void
update_stmt_eh_region (gimple *stmt)
{
  auto_vec<tree> exception_types;
  if (!stmt_throw_types (cfun, stmt, &exception_types))
    {
      return;
    }

  eh_region region;

  int lp_nr = lookup_stmt_eh_lp_fn (cfun, stmt);
  if (lp_nr <= 0)
    {
      return;
    }

  eh_landing_pad lp = get_eh_landing_pad_from_number (lp_nr);
  if (!lp)
    {
      return;
    }

  region = lp->region;
  eh_region resx_region = NULL;

  bool update = false;
  if (gimple_code (stmt) == GIMPLE_RESX)
    resx_region = get_eh_region_from_number (
        gimple_resx_region (as_a<gresx *> (stmt)));

  // Walk up the region tree
  while (region)
    {
      switch (region->type)
        {
        case ERT_CLEANUP:
          if (!update)
            return;

          if (gimple_code (stmt) == GIMPLE_RESX)
            {
              unlink_eh_region (resx_region);
              reinsert_eh_region (resx_region, region);
            }

          remove_stmt_from_eh_lp_fn (cfun, stmt);
          record_stmt_eh_region (region, stmt);
          return;

        case ERT_TRY:
          if (match_lp (region, &exception_types))
            {
              if (!update)
                return;
              if (gimple_code (stmt) == GIMPLE_RESX)
                {
                  unlink_eh_region (resx_region);
                  reinsert_eh_region (resx_region, region);
                }

              remove_stmt_from_eh_lp_fn (cfun, stmt);
              record_stmt_eh_region (region, stmt);
              return;
            }
          break;

        case ERT_MUST_NOT_THROW:
          // Undefined behavior, leave edge unchanged
          return;

        case ERT_ALLOWED_EXCEPTIONS:
          /* FIXME: match_lp will always return false.  */
          if (!match_lp (region, &exception_types))
            {
              return;
            }
          break;

        default:
          break;
        }
      region = region->outer;
      update = true;
    }

  if (!update)
    return;

  if (gimple_code (stmt) == GIMPLE_RESX)
    {
      unlink_eh_region (resx_region);
      reinsert_eh_region (resx_region, NULL);
    }
  remove_stmt_from_eh_lp_fn (cfun, stmt);
}