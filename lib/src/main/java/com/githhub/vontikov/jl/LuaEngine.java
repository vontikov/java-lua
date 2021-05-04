package com.githhub.vontikov.jl;

import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;

import javax.annotation.concurrent.NotThreadSafe;

/**
 * Provides the ability to execute Lua scripts from Java.
 *
 * Lua script can call Java functions registered via
 * {@link LuaEngine#registerFunction(String, Function)} method.
 *
 * @see https://www.lua.org/docs.html for Lua details.
 */
@NotThreadSafe
public class LuaEngine implements AutoCloseable {

    static {
        System.loadLibrary("jl");
    }

    private final Map<String, Function> functions = new HashMap<>();

    // native wrapper pointer.
    private final long np;

    /**
     * Constructs a new {@link LuaEngine}.
     */
    public LuaEngine() {
        np = init();
    }

    /**
     * Closes the instance, releases all underlying resources.
     */
    @Override
    public void close() {
        destroy();
    }

    /**
     * Sets the Lua engine log level.
     *
     * @param level is on of the: trace|debug|info|warn|error|fatal|off.
     */
    public void setLogLevel(final String level) {
        logLevel(level.toLowerCase());
    }

    /**
     * Registers a function, which may by called froma Lua script.
     *
     * @param name the function name.
     * @param func the function.
     */
    public void registerFunction(final String name, final Function func) {
        functions.put(name, func);
    }

    /**
     * Loads Lua script.
     * 
     * Appends the script to the already loaded script (if any).
     *
     * @param script the script to load
     * @throws ExecutionException
     */
    public void load(final String script) throws ExecutionException {
        final int r = loadScript(script);
        if (r != 0) {
            throw new ExecutionException(r, getLastError());
        }
    }

    /**
     * Loads and executes Lua script.
     *
     * @param script the script to load and execute
     * @throws ExecutionException
     */
    public void execute(final String script) throws ExecutionException {
        final int r = exec(script);
        if (r != 0) {
            throw new ExecutionException(r, getLastError());
        }
    }

    /**
     * Executes previously loaded Lua script.
     *
     * @throws ExecutionException
     */
    public void execute() throws ExecutionException {
        final int r = exec();
        if (r != 0) {
            throw new ExecutionException(r, getLastError());
        }
    }

    /**
     * This method is called from the Lua engine.
     *
     * @param args the first argument is a registered function name, the rest (if
     *             any) are passed to the function.
     * @return an arbitrary code returned back to the caller.
     * @throws FunctionException
     */
    private int callback(Object... args) throws FunctionException {
        assert (args.length > 0);
        assert (args[0] instanceof String);

        final String fn = (String) args[0];
        final Function callback = functions.get(fn);
        if (callback == null) {
            throw new FunctionException(String.format("function not found: %s", fn));
        }
        return callback.apply(Arrays.copyOfRange(args, 1, args.length));
    }

    private native long init();

    private native void destroy();

    private native void logLevel(final String level);

    private native int exec(final String script);

    private native int exec();

    private native int loadScript(final String script);

    private native String getLastError();
}
