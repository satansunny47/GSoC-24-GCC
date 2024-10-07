// Helper function to check if a landing pad can handle an exception type
bool match_lp(eh_landing_pad lp, tree exception_type) {
    // Suppose get_landing_pad_types(lp) returns a vec<tree> of handled types for a landing pad
    vec<tree> handled_types = get_landing_pad_types(lp);
    for (unsigned i = 0; i < handled_types.length(); ++i) {
        if (handled_types[i] == exception_type) {
            return true;
        }
    }
    return false;
}

edge make_eh_edge(gimple *stmt) {
    basic_block src, dst;
    eh_landing_pad lp;
    int lp_nr;
    vec<tree> exception_types;

    // Lookup eh landing pad number
    lp_nr = lookup_stmt_eh_lp(stmt);
    if (lp_nr <= 0) {
        return NULL;
    }

    // Extract the types of exceptions that can be thrown by the statement
    if (!stmt_throw_types(cfun, stmt, exception_types)) {
        return NULL;
    }

    // Iterate over all landing pads and find the best match
    src = gimple_bb(stmt);

    // Iterate over all landing pads for the current function
    for (int i = 1; i <= cfun->eh->region_array.length(); ++i) {
        lp = get_eh_landing_pad_from_number(i);
        if (!lp) {
            continue;
        }

        dst = label_to_block(cfun, lp->post_landing_pad);

        // Check if this landing pad handles any of the exception types
        for (unsigned j = 0; j < exception_types.length(); ++j) {
            if (match_lp(lp, exception_types[j])) {
                // Create an edge to this landing pad
                return make_edge(src, dst, EDGE_EH);
            }
        }
    }

    return NULL;
}

###########################################################################################

// Determine if the given region is a landing pad
bool is_landing_pad(eh_region region) {
    return region && region->landing_pads != nullptr;
}

// Retrieve the list of allowed types from a landing pad region
tree get_landing_pad_types(eh_region region) {
    if (region && region->type == ERT_ALLOWED_EXCEPTIONS) {
        return region->u.allowed.type_list;
    }
    return NULL_TREE;
}

// Determine if two types are equivalent
bool types_are_equivalent(tree type1, tree type2) {
    return type1 == type2;
}

// Check if a landing pad can handle any of the given exception types
bool landing_pad_handles_exception(eh_region region, vec<tree>& exception_types) {
    tree type_list = get_landing_pad_types(region);

    for (tree t = type_list; t; t = TREE_CHAIN(t)) {
        tree type = TREE_VALUE(t);
        for (unsigned i = 0; i < exception_types.length(); ++i) {
            if (types_are_equivalent(type, exception_types[i])) {
                return true;
            }
        }
    }
    return false;
}

edge make_eh_edge(gimple *stmt) {
    basic_block src, dst;
    eh_region region;
    int lp_nr;
    vec<tree> exception_types;

    lp_nr = lookup_stmt_eh_lp(stmt);
    if (lp_nr <= 0) {
        return NULL;
    }

    if (!stmt_throw_types(cfun, stmt, exception_types)) {
        return NULL;
    }

    src = gimple_bb(stmt);

    // Get the specific landing pad region using the lp_nr
    region = get_eh_landing_pad_from_number(lp_nr);
    if (!region || !is_landing_pad(region)) {
        return NULL;
    }

    dst = label_to_block(cfun, region->landing_pads->post_landing_pad);

    if (landing_pad_handles_exception(region, exception_types)) {
        return make_edge(src, dst, EDGE_EH);
    }

    return NULL;
}


