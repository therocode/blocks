#pragma once
#include <angelscript.h>
#include <cstdio>
#include "scriptengine.h"
#include <assert.h>

void parameterFunction(asIScriptContext* context, std::size_t index, int value);
 
void parameterFunction(asIScriptContext* context, std::size_t index, float value);

void parameterFunction(asIScriptContext* context, std::size_t index, bool value);

template <std::size_t I, typename... Types>
struct ParameterHelper;

template <std::size_t I, typename Type, typename... Types>
struct ParameterHelper<I, Type, Types...> 
{
    static void do_(asIScriptContext* context, Type first, Types... rest)
    {
        parameterFunction(context, I, first);
        ParameterHelper<I + 1, Types...>::do_(context, rest...);
    }
};

template <std::size_t I>
struct ParameterHelper<I> 
{
    static void do_(asIScriptContext* context)
    {
    }
};

template <typename... Types>
class ScriptCallback
{
    public:
        ScriptCallback(ScriptEngine& engine) : mEngine(engine), mFunction(nullptr)
        {
        }

        void setFunction(asIScriptFunction* function)
        {
            mFunction = function;
        }

        void execute(Types... parameters)
        {
            if(mFunction)
            {
                asIScriptContext* context = mEngine.requestContext();
                int32_t r = context->Prepare(mFunction); assert( r >= 0 );
                ParameterHelper<0, Types...>::do_(context, parameters...);
                r = context->Execute(); assert( r >= 0 );
                mEngine.freeContext(context);
            }
        }

    private:
        ScriptEngine& mEngine;
        asIScriptFunction* mFunction;
};

template <typename... Types>
class ScriptMemberCallback
{
    public:
        ScriptMemberCallback(ScriptEngine& engine) : mEngine(engine), mFunction(nullptr)
        {
        }

        void setFunction(asIScriptFunction* function)
        {
            mFunction = function;
        }

        void execute(asIScriptObject* object, Types... parameters)
        {
            if(mFunction)
            {
                asIScriptContext* context = mEngine.requestContext();
                int32_t r = context->Prepare(mFunction); assert( r >= 0 );
                ParameterHelper<0, Types...>::do_(context, parameters...);
                context->SetObject(object);
                r = context->Execute(); assert( r >= 0 );
                mEngine.freeContext(context);
            }
        }

    private:
        ScriptEngine& mEngine;
        asIScriptFunction* mFunction;
};
