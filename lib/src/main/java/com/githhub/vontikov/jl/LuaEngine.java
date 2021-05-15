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

    private static final String DEFAULT_NS = "java";

    private final Map<String, Map<String, Function>> functions = new HashMap<>();

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
        registerFunction(DEFAULT_NS, name, func);
    }

    public void registerFunction(final String ns, final String name, final Function func) {
        functions.computeIfAbsent(ns, k -> new HashMap<>()).put(name, func);
        registerFunction(ns, name);
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
     */
    private int callback(Object... args) {
        assert (args.length > 1);
        // namespace
        assert (args[0] instanceof String);
        // function name
        assert (args[1] instanceof String);

        final String ns = (String) args[0];
        final String fn = (String) args[1];

        final Function cb = functions.get(ns).get(fn);
        return cb.apply(Arrays.copyOfRange(args, 2, args.length));
    }

    private native long init() throws RuntimeException;

    private native void destroy() throws RuntimeException;;

    private native void logLevel(final String level) throws RuntimeException;;

    private native void registerFunction(final String ns, final String fn) throws RuntimeException;

    private native int exec(final String script) throws RuntimeException;;

    private native int exec() throws RuntimeException;;

    private native int loadScript(final String script) throws RuntimeException;;

    private native String getLastError() throws RuntimeException;;
}
