// Check if a landing pad can handle any of the given exception types
static bool
match_lp (eh_region region, vec<tree> *exception_types)
{
  // Ensure the region is of type ERT_TRY
  if (region && region->type == ERT_TRY)
    {
      eh_catch_d *catch_handler = region->u.eh_try.first_catch;

      while (catch_handler)
        {
          tree type_list = catch_handler->type_list;

          if (type_list == NULL)
            {
              return true;
            }

          for (tree t = type_list; t; t = TREE_CHAIN (t))
            {
              tree type = TREE_VALUE (t);
              for (unsigned i = 0; i < exception_types->length (); ++i)
                {
                  // match found or a catch-all handler (NULL)
                  if (!type
                      || same_or_derived_type (type, (*exception_types)[i]))
                    {
                      return true;
                    }
                }
            }
          catch_handler = catch_handler->next_catch;
        }
    }
  return false;
}