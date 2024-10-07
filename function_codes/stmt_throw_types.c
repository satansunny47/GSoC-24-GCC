/* Determine which types can be thrown by a GIMPLE statement and convert them
 * to compile-time types */
bool
stmt_throw_types (function *, gimple *stmt, vec<tree> *ret_vector)
{
  if (!flag_exceptions)
    {
      return false;
    }
  bool type_exists = true;

  switch (gimple_code (stmt))
    {
    case GIMPLE_RESX:
      type_exists = extract_types_for_resx (as_a<gresx *> (stmt), ret_vector);
      return type_exists;

    case GIMPLE_CALL:
      type_exists = extract_types_for_call (as_a<gcall *> (stmt), ret_vector);
      /* FIXME: if type exists we should have always vector nonempty.  */
      return type_exists && !ret_vector->is_empty ();

    default:
      return false;
    }
}