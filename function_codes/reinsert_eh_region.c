static void
reinsert_eh_region (eh_region region, eh_region new_outer)
{
  region->outer = new_outer;

  // Insert region as the inner of new_outer, or at the top of the tree
  if (new_outer)
    {
      region->next_peer = new_outer->inner;
      new_outer->inner = region;
    }
  else
    {
      region->next_peer = cfun->eh->region_tree;
      cfun->eh->region_tree = region;
    }
}