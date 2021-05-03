package com.githhub.vontikov.jl;

/**
 * Represents a function called from a Lua script.
 */
@FunctionalInterface
public interface Function {

    /**
     * Applies this function to the given arguments.
     *
     * @param args the function arguments
     * @return the function result, which returns to the caller script.
     */
    int apply(Object... args);
}
