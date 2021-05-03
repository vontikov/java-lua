package com.githhub.vontikov.jl;

/**
 * The class indicates that a problem occurred on the Java side while a
 * {@link Function} was called.
 */
public class FunctionException extends RuntimeException {

    private static final long serialVersionUID = 1L;

    /**
     * Constructs a new {@link FunctionException} with the specified detail message.
     *
     * @param message the detail message.
     */
    public FunctionException(final String message) {
        super(message);
    }
}
