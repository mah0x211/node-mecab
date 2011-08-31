#include <node.h>
#include <node_buffer.h>
#include <cstring>
#include <mecab.h>

using namespace v8;
using namespace node;

// MARK: mecab_node_t
#define NODE_PREV_SYMBOL String::NewSymbol("prev")
#define NODE_NEXT_SYMBOL String::NewSymbol("next")
#define NODE_ENEXT_SYMBOL String::NewSymbol("enext")
#define NODE_BNEXT_SYMBOL String::NewSymbol("bnext")

#define NODE_FORMAT_SYMBOL String::NewSymbol("format_node")
#define NODE_SURFACE_SYMBOL String::NewSymbol("surface")
#define NODE_FEATURE_SYMBOL String::NewSymbol("feature")
#define NODE_ID_SYMBOL String::NewSymbol("id")
#define NODE_LENGTH_SYMBOL String::NewSymbol("length")
#define NODE_RLENGTH_SYMBOL String::NewSymbol("rlength")
#define NODE_RCATTR_SYMBOL String::NewSymbol("rcAttr")
#define NODE_LCATTR_SYMBOL String::NewSymbol("lcAttr")
#define NODE_POSID_SYMBOL String::NewSymbol("posid")
#define NODE_CHAR_TYPE_SYMBOL String::NewSymbol("char_type")
#define NODE_STAT_SYMBOL String::NewSymbol("stat")
#define NODE_ISBEST_SYMBOL String::NewSymbol("isbest")
#define NODE_SENTENCE_LENGTH_SYMBOL String::NewSymbol("sentence_length")
#define NODE_ALPHA_SYMBOL String::NewSymbol("alpha")
#define NODE_BETA_SYMBOL String::NewSymbol("beta")
#define NODE_PROB_SYMBOL String::NewSymbol("prob")
#define NODE_WCOST_SYMBOL String::NewSymbol("wcost")
#define NODE_COST_SYMBOL String::NewSymbol("cost")

class MeCabNode : public ObjectWrap
{
	public:
		MeCabNode() : ObjectWrap(){
			mecab_ = NULL;
			mnode_ = NULL;
		};
		~MeCabNode(){};
		
		static void Initialize( Handle<Object> target );
		static Handle<Value> New( mecab_t *mecab, const mecab_node_t *mnode );
	private:
		mecab_t *mecab_;
		const mecab_node_t *mnode_;
		static Persistent<FunctionTemplate> constructor_template;
		
		static Handle<Value> New( const Arguments& args );
		static Handle<Value> getter( Local<String> property, const AccessorInfo& info );
		
		static Handle<Value> begin_node_list( const Arguments& args );
		static Handle<Value> end_node_list( const Arguments& args );
};

Persistent<FunctionTemplate> MeCabNode::constructor_template;

void MeCabNode::Initialize( Handle<Object> target )
{
	HandleScope scope;
	Local<FunctionTemplate> t = FunctionTemplate::New( New );
	
	t->SetClassName( String::NewSymbol("MeCabNode") );
	t->InstanceTemplate()->SetInternalFieldCount(1);
	
	NODE_SET_PROTOTYPE_METHOD( t, "begin_node_list", begin_node_list );
	NODE_SET_PROTOTYPE_METHOD( t, "end_node_list", end_node_list );
	
	// set accessor: getter
	t->PrototypeTemplate()->SetAccessor( NODE_PREV_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( NODE_NEXT_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( NODE_ENEXT_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( NODE_BNEXT_SYMBOL, getter );
	
	t->PrototypeTemplate()->SetAccessor( NODE_FORMAT_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( NODE_SURFACE_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( NODE_FEATURE_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( NODE_ID_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( NODE_LENGTH_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( NODE_RLENGTH_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( NODE_RCATTR_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( NODE_LCATTR_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( NODE_POSID_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( NODE_CHAR_TYPE_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( NODE_STAT_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( NODE_ISBEST_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( NODE_SENTENCE_LENGTH_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( NODE_ALPHA_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( NODE_BETA_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( NODE_PROB_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( NODE_WCOST_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( NODE_COST_SYMBOL, getter );
	// create template
	constructor_template = Persistent<FunctionTemplate>::New(t);
	// finalize
	target->Set( String::NewSymbol("MeCabNode"), constructor_template->GetFunction() );
}

Handle<Value> MeCabNode::New( mecab_t *mecab, const mecab_node_t *mnode )
{
	HandleScope scope;
	Local<Value> arg = Integer::NewFromUnsigned(0);
	Local<Object> obj = constructor_template->GetFunction()->NewInstance(1, &arg);

	MeCabNode *n = ObjectWrap::Unwrap<MeCabNode>(obj);
	n->mecab_ = mecab;
	n->mnode_ = mnode;
	
	return obj;
}

Handle<Value> MeCabNode::New( const Arguments& args )
{
	if( !args.IsConstructCall() ){
		return FromConstructorTemplate( constructor_template, args );
	}
	
	HandleScope scope;
	MeCabNode *n = new MeCabNode();
	
	n->Wrap( args.Holder() );
	
	return args.This();
}


Handle<Value> MeCabNode::begin_node_list( const Arguments& args )
{
	HandleScope scope;
	MeCabNode *n = ObjectWrap::Unwrap<MeCabNode>( args.This() );
	unsigned int idx = args[0]->Uint32Value();
	
	if( n->mnode_->stat == MECAB_BOS_NODE && 
		idx <= n->mnode_->sentence_length &&
		n->mnode_->begin_node_list[idx] ){
		return scope.Close( MeCabNode::New( n->mecab_, n->mnode_->begin_node_list[idx] ) );
	}
	
	return scope.Close( Undefined() );
}

Handle<Value> MeCabNode::end_node_list( const Arguments& args )
{
	HandleScope scope;
	MeCabNode *n = ObjectWrap::Unwrap<MeCabNode>( args.This() );
	unsigned int idx = args[0]->Uint32Value();
	
	if( n->mnode_->stat == MECAB_BOS_NODE && 
		idx <= n->mnode_->sentence_length &&
		n->mnode_->end_node_list[idx] ){
		return scope.Close( MeCabNode::New( n->mecab_, n->mnode_->end_node_list[idx] ) );
	}
	
	return scope.Close( Undefined() );
}

Handle<Value> MeCabNode::getter( Local<String> property, const AccessorInfo& info )
{
	MeCabNode* n = ObjectWrap::Unwrap<MeCabNode>(info.This());
	const mecab_node_t *mnode = n->mnode_;
	
	if( property == NODE_PREV_SYMBOL )
	{
		if( !n->mnode_->prev ){
			return Undefined();
		}
		return MeCabNode::New( n->mecab_, n->mnode_->prev );
	}
	else if( property == NODE_NEXT_SYMBOL )
	{
		if( !n->mnode_->next ){
			return Undefined();
		}
		return MeCabNode::New( n->mecab_, n->mnode_->next );
	}
	else if( property == NODE_ENEXT_SYMBOL )
	{
		if( !n->mnode_->enext ){
			return Undefined();
		}
		return MeCabNode::New( n->mecab_, n->mnode_->enext );
	}
	else if( property == NODE_BNEXT_SYMBOL )
	{
		if( !n->mnode_->bnext ){
			return Undefined();
		}
		return MeCabNode::New( n->mecab_, n->mnode_->bnext );
	}
	else if( property == NODE_FORMAT_SYMBOL ){
		const char *res = mecab_format_node( n->mecab_, n->mnode_ );
		if( !res ){
			return ThrowException( Exception::Error( String::New( mecab_strerror(n->mecab_) ) ) );
		}
		return String::New( res );
	}
	else if( property == NODE_SURFACE_SYMBOL ){
		return String::New( mnode->surface );
	}
	else if( property == NODE_FEATURE_SYMBOL ){
		return String::New( mnode->feature );
	}
	else if( property == NODE_LENGTH_SYMBOL ){
		return Int32::New( mnode->length );
	}
	else if( property == NODE_RLENGTH_SYMBOL ){
		return Int32::New( mnode->rlength );
	}
	else if( property == NODE_ID_SYMBOL ){
		return Int32::New( mnode->id );
	}
	else if( property == NODE_RCATTR_SYMBOL ){
		return Int32::New( mnode->rcAttr );
	}
	else if( property == NODE_LCATTR_SYMBOL ){
		return Int32::New( mnode->lcAttr );
	}
	else if( property == NODE_POSID_SYMBOL ){
		return Int32::New( mnode->posid );
	}
	else if( property == NODE_CHAR_TYPE_SYMBOL ){
		return Int32::New( mnode->char_type );
	}
	else if( property == NODE_STAT_SYMBOL ){
		return Int32::New( mnode->stat );
	}
	else if( property == NODE_ISBEST_SYMBOL ){
		return Int32::New( mnode->isbest );
	}
	else if( property == NODE_SENTENCE_LENGTH_SYMBOL ){
		return Int32::New( mnode->sentence_length );
	}
	else if( property == NODE_ALPHA_SYMBOL ){
		return Number::New( mnode->alpha );
	}
	else if( property == NODE_BETA_SYMBOL ){
		return Number::New( mnode->beta );
	}
	else if( property == NODE_PROB_SYMBOL ){
		return Number::New( mnode->prob );
	}
	else if( property == NODE_WCOST_SYMBOL ){
		return Int32::New( mnode->wcost );
	}
	else if( property == NODE_COST_SYMBOL ){
		return Int32::New( mnode->cost );
	}
	else{
		return ThrowException(Exception::Error( property ) );
	}
}

// MARK: mecab_dictionary_info_t
#define DICT_FILENAME_SYMBOL String::NewSymbol("filename")
#define DICT_CHARSET_SYMBOL String::NewSymbol("charset")
#define DICT_SIZE_SYMBOL String::NewSymbol("size")
#define DICT_TYPE_SYMBOL String::NewSymbol("type")
#define DICT_LSIZE_SYMBOL String::NewSymbol("lsize")
#define DICT_RSIZE_SYMBOL String::NewSymbol("rsize")
#define DICT_VERSION_SYMBOL String::NewSymbol("version")

class MeCabDictInfo : public ObjectWrap
{
	public:
		MeCabDictInfo() : ObjectWrap(){
			dict_ = NULL;
		};
		~MeCabDictInfo(){};
		
		static void Initialize( Handle<Object> target );
		static Handle<Value> New( const mecab_dictionary_info_t *dict );
	private:
		const mecab_dictionary_info_t *dict_;
		static Persistent<FunctionTemplate> constructor_template;
		
		static Handle<Value> New( const Arguments& args );
		static Handle<Value> prev( const Arguments& args );
		static Handle<Value> next( const Arguments& args );
		static Handle<Value> getter( Local<String> property, const AccessorInfo& info );
};

Persistent<FunctionTemplate> MeCabDictInfo::constructor_template;

void MeCabDictInfo::Initialize( Handle<Object> target )
{
	HandleScope scope;
	Local<FunctionTemplate> t = FunctionTemplate::New( New );
	
	t->SetClassName( String::NewSymbol("MeCabDictInfo") );
	t->InstanceTemplate()->SetInternalFieldCount(1);
	
	NODE_SET_PROTOTYPE_METHOD( t, "next", next );
	// set accessor: getter
	t->PrototypeTemplate()->SetAccessor( DICT_FILENAME_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( DICT_CHARSET_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( DICT_SIZE_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( DICT_TYPE_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( DICT_LSIZE_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( DICT_RSIZE_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( DICT_VERSION_SYMBOL, getter );
	// create template
	constructor_template = Persistent<FunctionTemplate>::New(t);
	// finalize
	target->Set( String::NewSymbol("MeCabDictInfo"), constructor_template->GetFunction() );
}

Handle<Value> MeCabDictInfo::New( const mecab_dictionary_info_t *dict )
{
	HandleScope scope;
	Local<Value> arg = Integer::NewFromUnsigned(0);
	Local<Object> obj = constructor_template->GetFunction()->NewInstance(1, &arg);
	
	MeCabDictInfo *d = ObjectWrap::Unwrap<MeCabDictInfo>(obj);
	d->dict_ = dict;
	
	return obj;
}

Handle<Value> MeCabDictInfo::New( const Arguments& args )
{
	if( !args.IsConstructCall() ){
		return FromConstructorTemplate( constructor_template, args );
	}
	HandleScope scope;
	MeCabDictInfo *d = new MeCabDictInfo();
	Local<Object> thisObj = args.This();
	
	d->Wrap( args.This() );
	
	return args.This();
}

Handle<Value> MeCabDictInfo::next( const Arguments& args )
{
	HandleScope scope;
	MeCabDictInfo *d = ObjectWrap::Unwrap<MeCabDictInfo>( args.This() );
	mecab_dictionary_info_t *next = d->dict_->next;
	
	if( !next ){
		return scope.Close( Undefined() );
	}
	return scope.Close( MeCabDictInfo::New( next ) );
}


Handle<Value> MeCabDictInfo::getter( Local<String> property, const AccessorInfo& info )
{
	MeCabDictInfo* d = ObjectWrap::Unwrap<MeCabDictInfo>(info.This());
	const mecab_dictionary_info_t *dict = d->dict_;
	
	if( property == DICT_FILENAME_SYMBOL ){
		return String::New( dict->filename );
	}
	else if( property == DICT_CHARSET_SYMBOL ){
		return String::New( dict->charset );
	}
	else if( property == DICT_SIZE_SYMBOL ){
		return Uint32::New( dict->size );
	}
	else if( property == DICT_TYPE_SYMBOL ){
		return Integer::New( dict->type );
	}
	else if( property == DICT_LSIZE_SYMBOL ){
		return Uint32::New( dict->lsize );
	}
	else if( property == DICT_RSIZE_SYMBOL ){
		return Uint32::New( dict->rsize );
	}
	else if( property == DICT_VERSION_SYMBOL ){
		return Int32::New( dict->version );
	}
	else{
		return ThrowException(Exception::Error( property ) );
	}
}


// MARK: mecab_t
#define PARTIAL_SYMBOL String::NewSymbol("partial")
#define THETA_SYMBOL String::NewSymbol("theta")
#define LATTICE_LEVEL_SYMBOL String::NewSymbol("lattice_level")
#define ALL_MORPHS_SYMBOL String::NewSymbol("all_morphs")
#define NOR_NODE_SYMBOL String::NewSymbol("NOR_NODE")
#define UNK_NODE_SYMBOL String::NewSymbol("UNK_NODE")

#define BOS_NODE_SYMBOL String::NewSymbol("BOS_NODE")
#define EOS_NODE_SYMBOL String::NewSymbol("EOS_NODE")
#define EON_NODE_SYMBOL String::NewSymbol("EON_NODE")

#define USR_DIC_SYMBOL String::NewSymbol("USR_DIC")
#define SYS_DIC_SYMBOL String::NewSymbol("SYS_DIC")
#define UNK_DIC_SYMBOL String::NewSymbol("UNK_DIC")

class MeCab : public ObjectWrap
{
	public:
		static void Initialize( Handle<Object> target );
		MeCab(){
			mecab_ = NULL;
		};
		~MeCab(){
			mecab_destroy( mecab_ );
		};
		
	private:
		mecab_t *mecab_;
		String *input_;
		
		static Handle<Value> New (const Arguments& args);
		static Handle<Value> strerror( const Arguments& args );
		
		static Handle<Value> getter( Local<String> property, const AccessorInfo& info );
		static void setter( Local<String> property, Local<Value> value, const AccessorInfo& info );
		
		static Handle<Value> dictionary_info( const Arguments& args );
		
		static Handle<Value> sparse_tostr( const Arguments& args );
		static Handle<Value> sparse_tonode( const Arguments& args );
		
		static Handle<Value> nbest_sparse_tostr( const Arguments& args );
		static Handle<Value> nbest_init( const Arguments& args );
		static Handle<Value> nbest_next_tostr( const Arguments& args );
		static Handle<Value> nbest_next_tonode( const Arguments& args );
};


Handle<Value> MeCab::New( const Arguments& args )
{
	HandleScope scope;
	MeCab *m = new MeCab();
	
	m->Wrap( args.Holder() );
	if( !args[0]->IsString() ){
		return ThrowException( Exception::Error( String::New( "invalid arguments" ) ) );
	}
	else if( !( m->mecab_ = mecab_new2( *String::Utf8Value( args[0]->ToString() ) ) ) ){
		return ThrowException( Exception::Error( String::New( mecab_strerror(NULL) ) ) );
	}
	
	// mecab->resolver = Handle<Function>::Cast( Null() );
	return args.This();
}

Handle<Value> MeCab::strerror( const Arguments& args )
{
	HandleScope scope;
	MeCab *m = ObjectWrap::Unwrap<MeCab>(args.This());
	
	return scope.Close( String::New( mecab_strerror( m->mecab_ ) ) );
}


Handle<Value> MeCab::sparse_tostr( const Arguments& args )
{
	HandleScope scope;
	MeCab *m = ObjectWrap::Unwrap<MeCab>(args.This());
	const char *res = mecab_sparse_tostr( m->mecab_, *String::Utf8Value( args[0]->ToString() ) );
	
	if( !res ){
		return ThrowException( Exception::Error( String::New( mecab_strerror(m->mecab_) ) ) );
	}
	return scope.Close( String::New( res ) );
}

Handle<Value> MeCab::sparse_tonode( const Arguments& args )
{
	HandleScope scope;
	MeCab *m = ObjectWrap::Unwrap<MeCab>(args.This());
	const mecab_node_t *n = mecab_sparse_tonode( m->mecab_, *String::Utf8Value( args[0]->ToString() ) );
	
	if( !n ){
		return ThrowException( Exception::Error( String::New( mecab_strerror(m->mecab_) ) ) );
	}
	
	return scope.Close( MeCabNode::New( m->mecab_, n ) );
}

Handle<Value> MeCab::nbest_sparse_tostr( const Arguments& args )
{
	HandleScope scope;
	MeCab *m = ObjectWrap::Unwrap<MeCab>(args.This());
	const char *res = mecab_nbest_sparse_tostr( m->mecab_, (int)args[0]->Uint32Value(), *String::Utf8Value( args[1]->ToString() ) );
	
	if( !res ){
		return ThrowException( Exception::Error( String::New( mecab_strerror(m->mecab_) ) ) );
	}
	return scope.Close( String::New( res ) );
}


Handle<Value> MeCab::nbest_init( const Arguments& args )
{
	HandleScope scope;
	MeCab *m = ObjectWrap::Unwrap<MeCab>(args.This());
	int rc = mecab_nbest_init( m->mecab_, *String::Utf8Value( args[0]->ToString() ) );
	
	if( !rc ){
		return ThrowException( Exception::Error( String::New( mecab_strerror(m->mecab_) ) ) );
	}
	return scope.Close( Integer::New( rc ) );
}

Handle<Value> MeCab::nbest_next_tostr( const Arguments& args )
{
	HandleScope scope;
	MeCab *m = ObjectWrap::Unwrap<MeCab>(args.This());
	const char *res = mecab_nbest_next_tostr( m->mecab_ );
	
	if( !res )
	{
		if( !( res = mecab_strerror( m->mecab_ ) ) &&
			strcmp( res, "no more results" ) != 0 ){
			return ThrowException( Exception::Error( String::New( res ) ) );
		}
		else {
			return scope.Close( Undefined() );
		}
	}
	
	return scope.Close( String::New( res ) );
}

Handle<Value> MeCab::nbest_next_tonode( const Arguments& args )
{
	HandleScope scope;
	MeCab *m = ObjectWrap::Unwrap<MeCab>(args.This());
	const mecab_node_t *mnode = mecab_nbest_next_tonode( m->mecab_ );
	
	if( !mnode ){
		return ThrowException( Exception::Error( String::New( mecab_strerror(m->mecab_) ) ) );
	}
	
	return scope.Close( MeCabNode::New( m->mecab_, mnode ) );
}

Handle<Value> MeCab::dictionary_info( const Arguments& args )
{
	HandleScope scope;
	MeCab *m = ObjectWrap::Unwrap<MeCab>(args.This());
	const mecab_dictionary_info_t *d = mecab_dictionary_info( m->mecab_ );
	
	if( !d ){
		return ThrowException( Exception::Error( String::New( mecab_strerror(m->mecab_) ) ) );
	}
	
	return scope.Close( MeCabDictInfo::New( d ) );
}


Handle<Value> MeCab::getter( Local<String> property, const AccessorInfo& info )
{
	Local<Object> self = info.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));	
	void* ptr = wrap->Value();
	MeCab *m = static_cast<MeCab*>(ptr);
	
	if( property == NOR_NODE_SYMBOL ){
		return Number::New( MECAB_NOR_NODE );
	}
	else if( property == UNK_NODE_SYMBOL ){
		return Number::New( MECAB_UNK_NODE );
	}
	else if( property == BOS_NODE_SYMBOL ){
		return Number::New( MECAB_BOS_NODE );
	}
	else if( property == EOS_NODE_SYMBOL ){
		return Number::New( MECAB_EOS_NODE );
	}
	else if( property == EON_NODE_SYMBOL ){
		return Number::New( MECAB_EON_NODE );
	}
	else if( property == USR_DIC_SYMBOL ){
		return Number::New( MECAB_USR_DIC );
	}
	else if( property == SYS_DIC_SYMBOL ){
		return Number::New( MECAB_SYS_DIC );
	}
	else if( property == UNK_DIC_SYMBOL ){
		return Number::New( MECAB_UNK_DIC );
	}
	else if( property == PARTIAL_SYMBOL ){
		return Number::New( mecab_get_partial( m->mecab_ ) );
	}
	else if( property == THETA_SYMBOL ){
		return Number::New( mecab_get_theta( m->mecab_ ) );
	}
	else if( property == LATTICE_LEVEL_SYMBOL ){
		return Number::New( mecab_get_lattice_level( m->mecab_ ) );
	}
	else if( property == ALL_MORPHS_SYMBOL ){
		return Number::New( mecab_get_all_morphs( m->mecab_ ) );
	}
	else{
		return ThrowException(Exception::Error( String::New( "invalid property" ) ) );
	}
}

void MeCab::setter( Local<String> property, Local<Value> value, const AccessorInfo& info )
{
	MeCab *m = ObjectWrap::Unwrap<MeCab>(info.This());
	
	if( property == PARTIAL_SYMBOL ){
		mecab_set_partial( m->mecab_, value->IntegerValue() );
	}
	else if( property == THETA_SYMBOL ){
		mecab_set_theta( m->mecab_, value->NumberValue() );
	}
	else if( property == LATTICE_LEVEL_SYMBOL ){
		mecab_set_lattice_level( m->mecab_, value->IntegerValue() );
	}
	else if( property == ALL_MORPHS_SYMBOL ){
		mecab_set_all_morphs( m->mecab_, value->IntegerValue() );
	}
}

void MeCab::Initialize( Handle<Object> target )
{
	HandleScope scope;
	Local<FunctionTemplate> t = FunctionTemplate::New( New );
	
	t->SetClassName( String::NewSymbol("MeCab") );
	t->InstanceTemplate()->SetInternalFieldCount(1);
	NODE_SET_PROTOTYPE_METHOD( t, "strerror", strerror );
	
	// set accessor: getter/setter
	t->PrototypeTemplate()->SetAccessor( NOR_NODE_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( UNK_NODE_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( BOS_NODE_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( EOS_NODE_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( EON_NODE_SYMBOL, getter );
	
	t->PrototypeTemplate()->SetAccessor( USR_DIC_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( SYS_DIC_SYMBOL, getter );
	t->PrototypeTemplate()->SetAccessor( UNK_DIC_SYMBOL, getter );
	
	t->PrototypeTemplate()->SetAccessor( PARTIAL_SYMBOL, getter, setter );
	t->PrototypeTemplate()->SetAccessor( THETA_SYMBOL, getter, setter );
	t->PrototypeTemplate()->SetAccessor( LATTICE_LEVEL_SYMBOL, getter, setter );
	t->PrototypeTemplate()->SetAccessor( ALL_MORPHS_SYMBOL, getter, setter );
	
	
	// methods
	NODE_SET_PROTOTYPE_METHOD( t, "sparse_tostr", sparse_tostr );
	NODE_SET_PROTOTYPE_METHOD( t, "sparse_tonode", sparse_tonode );
	
	NODE_SET_PROTOTYPE_METHOD( t, "nbest_sparse_tostr", nbest_sparse_tostr );
	NODE_SET_PROTOTYPE_METHOD( t, "nbest_init", nbest_init );
	NODE_SET_PROTOTYPE_METHOD( t, "nbest_next_tostr", nbest_next_tostr );
	NODE_SET_PROTOTYPE_METHOD( t, "nbest_next_tonode", nbest_next_tonode );
	NODE_SET_PROTOTYPE_METHOD( t, "dictionary_info", dictionary_info );
	
	/*
	NODE_SET_METHOD( t, "dict_index", dict_index );
	NODE_SET_METHOD( t, "dict_gen", dict_gen );
	NODE_SET_METHOD( t, "cost_train", cost_train );
	NODE_SET_METHOD( t, "test_gen", test_gen );
	NODE_SET_METHOD( target, "system_eval", system_eval );
	*/
	
	// finalize
	target->Set( String::NewSymbol("Init"), t->GetFunction() );
}

Handle<Value> version( const Arguments& args ){
	HandleScope scope;
	return scope.Close( String::New( mecab_version() ) );
}

extern "C" 
void init( Handle<Object> target )
{
	HandleScope scope;
	MeCab::Initialize( target );
	MeCabNode::Initialize( target );
	MeCabDictInfo::Initialize( target );
	NODE_SET_METHOD( target, "version", version );
};
