package com.githhub.vontikov.jl;

/**
 * The class indicates that a problem occurred during a Lua script execution.
 */
public class ExecutionException extends Exception {

    private static final long serialVersionUID = 1L;

    private final LuaRetCode code;

    /**
     * Constructs a new {@link ExecutionException} with the specified code and
     * detail message.
     *
     * @param code    is the Lua return code.
     * @param message is the error message which describes the problem.
     *
     * @see https://www.lua.org/docs.html for the details.
     */
    public ExecutionException(final int code, final String message) {
        super(message);
        this.code = LuaRetCode.get(code);
    }

    public LuaRetCode getCode() {
        return code;
    }

    @Override
    public String toString() {
        return "ExecutionException [code=" + code + ", message=" + getLocalizedMessage() + "]";
    }
}
