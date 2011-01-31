/* This code is PUBLIC DOMAIN, and is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND. See the accompanying
 * LICENSE file.
 */
#include <stdio.h>
#include <v8.h>
#include <node.h>
#include <GeoIP.h>
#include <boost/format.hpp>

#include <unistd.h>

using namespace node;
using namespace v8;

#define REQ_FUN_ARG(I, VAR)                                             \
  if (args.Length() <= (I) || !args[I]->IsFunction())                   \
    return ThrowException(Exception::TypeError(                         \
                  String::New("Argument " #I " must be a function")));  \
  Local<Function> VAR = Local<Function>::Cast(args[I]);

class GeoIPWrapper: ObjectWrap
{
private:
  GeoIP *gi;
public:

  static Persistent<FunctionTemplate> s_ct;
  static void Init(Handle<Object> target)
  {
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(New);

    s_ct = Persistent<FunctionTemplate>::New(t);
    s_ct->InstanceTemplate()->SetInternalFieldCount(1);
    s_ct->SetClassName(String::NewSymbol("GeoIP"));

    NODE_SET_PROTOTYPE_METHOD(s_ct, "open", Open);
    NODE_SET_PROTOTYPE_METHOD(s_ct, "query", Query);
    NODE_SET_PROTOTYPE_METHOD(s_ct, "advQuery", AdvQuery);


    target->Set(String::NewSymbol("GeoIP"),
                s_ct->GetFunction());
  }

  GeoIPWrapper() : gi(0)
  {
  }

  ~GeoIPWrapper()
  {
  }


  static Handle<Value> New(const Arguments& args)
  {
    HandleScope scope;
    GeoIPWrapper* self = new GeoIPWrapper();
    self->Wrap(args.This());
    return args.This();
  }

  static Handle<Value> Open(const Arguments& args)
  {
    HandleScope scope;

	if (args.Length() < 2 || !args[0]->IsString()) {
		return ThrowException(
			Exception::TypeError(
				String::New("Required argument: path to database, callback(err)")));
	}

	String::Utf8Value dbpath(args[0]->ToString());
	REQ_FUN_ARG(1, cb);

    GeoIPWrapper* self = ObjectWrap::Unwrap<GeoIPWrapper>(args.This());

    self->gi = GeoIP_open(*dbpath, GEOIP_MEMORY_CACHE);

    Local<Value> argv[1];

	if (self->gi == NULL) {
		argv[0] = String::New(boost::str(boost::format("Could not open db at %1%") % *dbpath).c_str());
	} else {
    	argv[0] = Local<Primitive>::New(Null());
    }

    TryCatch try_catch;

    cb->Call(Context::GetCurrent()->Global(), 1, argv);

    if (try_catch.HasCaught()) {
      FatalException(try_catch);
    }

    return Undefined();
  }

  static Handle<Value> Query(const Arguments& args)
  {
    HandleScope scope;

    GeoIPWrapper* self = ObjectWrap::Unwrap<GeoIPWrapper>(args.This());

	if (args.Length() < 1 || !args[0]->IsString()) {
		return ThrowException(
			Exception::TypeError(
				String::New("Required argument: <ip address>")));
	}

	String::Utf8Value query(args[0]->ToString());

	if (self->gi == 0) {
		return v8::ThrowException(v8::String::New("GeoIP library is not loaded. Maybe you forgot to call open()"));
	}

	const char *code = GeoIP_country_code_by_addr(self->gi, *query);
	if (code == 0) {
		return Undefined();
	} else {
		Handle<Value> result = String::New(code);
		return scope.Close(result);
	}
  }

  static Handle<Value> AdvQuery(const Arguments& args)
  {
    HandleScope scope;

    GeoIPWrapper* self = ObjectWrap::Unwrap<GeoIPWrapper>(args.This());

	if (args.Length() < 1 || !args[0]->IsString()) {
		return ThrowException(
			Exception::TypeError(
				String::New("Required argument: <ip address>")));
	}

	String::Utf8Value query(args[0]->ToString());

	if (self->gi == 0) {
		return v8::ThrowException(v8::String::New("GeoIP library is not loaded. Maybe you forgot to call open()"));
	}

	const char *code = GeoIP_country_code_by_addr(self->gi, *query);



	if (code == 0) {
		return Undefined();
	} else {
		Local<Array> result = Array::New();

		result->Set(
			String::New("country_code"),
			String::New(code)
		);

		// The GeoIP library contains two arrays which can be iterated to get country code => continent mapping
		// This operation is O(n) where n is the amount of countries in the database (currently 253)
		for (int i = 0; i <	(int) (sizeof(GeoIP_country_continent) / sizeof(GeoIP_country_continent[0])); i++) {
			if (strncmp(code, GeoIP_country_code[i], 2) == 0) {
				result->Set(
					String::New("continent"),
					String::New(GeoIP_country_continent[i])
				);

				break;
			}
		}


		return scope.Close(result);
	}
  }

};

Persistent<FunctionTemplate> GeoIPWrapper::s_ct;

extern "C" {
  void init (Handle<Object> target)
  {
    GeoIPWrapper::Init(target);
  }

  NODE_MODULE(GeoIPWrapper, init);
}
