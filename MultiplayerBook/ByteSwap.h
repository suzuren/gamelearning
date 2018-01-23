
#ifndef RoboCat_ByteSwap_h
#define RoboCat_ByteSwap_h

inline unsigned short ByteSwap2( unsigned short inData )
{
	return ( inData >> 8 ) | ( inData << 8 );
}

inline unsigned int ByteSwap4( unsigned int inData )
{
	return  ( ( inData >> 24 ) & 0x000000ff ) |
			( ( inData >>  8 ) & 0x0000ff00 ) |
			( ( inData <<  8 ) & 0x00ff0000 ) |
			( ( inData << 24 ) & 0xff000000 );
}

inline unsigned long long ByteSwap8( unsigned long long inData )
{
	return  ( ( inData >> 56 ) & 0x00000000000000ff ) |
			( ( inData >> 40 ) & 0x000000000000ff00 ) |
			( ( inData >> 24 ) & 0x0000000000ff0000 ) |
			( ( inData >>  8 ) & 0x00000000ff000000 ) |
			( ( inData <<  8 ) & 0x000000ff00000000 ) |
			( ( inData << 24 ) & 0x0000ff0000000000 ) |
			( ( inData << 40 ) & 0x00ff000000000000 ) |
			( ( inData << 56 ) & 0xff00000000000000 );
}


template < typename tFrom, typename tTo >
class TypeAliaser
{
public:
	TypeAliaser( tFrom inFromValue ) :
		mAsFromType( inFromValue ) {}
	tTo& Get() { return mAsToType; }
	
	union
	{
		tFrom 	mAsFromType;
		tTo		mAsToType;
	};
};


template <typename T, size_t tSize > class ByteSwapper;

//specialize for 1...
template <typename T>
class ByteSwapper< T, 1 >
{
public:
	T Swap( T inData ) const
	{
		return inData;
	}
};


//specialize for 2...
template <typename T>
class ByteSwapper< T, 2 >
{
public:
	T Swap( T inData ) const
	{
		unsigned short result =
			ByteSwap2( TypeAliaser< T, unsigned short >( inData ).Get() );
		return TypeAliaser< unsigned short, T >( result ).Get();
	}
};

//specialize for 4...
template <typename T>
class ByteSwapper< T, 4 >
{
public:
	T Swap( T inData ) const
	{
		unsigned int result =
			ByteSwap4( TypeAliaser< T, unsigned int >( inData ).Get() );
		return TypeAliaser< unsigned int, T >( result ).Get();
	}
};


//specialize for 8...
template <typename T>
class ByteSwapper< T, 8 >
{
public:
	T Swap( T inData ) const
	{
		unsigned long long result =
			ByteSwap8( TypeAliaser< T, unsigned long long >( inData ).Get() );
		return TypeAliaser< unsigned long long, T >( result ).Get();
	}
};

template < typename T >
T ByteSwap( T inData )
{
	return ByteSwapper< T, sizeof( T ) >().Swap( inData );
}

inline void TestByteSwap()
{
	unsigned int test = 0x12345678;
	float floatTest = 1.f;
	
	printf( "swapped 0x%x is 0x%x\n", test, ByteSwap( test ) );
	printf( "swapped %f is %f\n", floatTest, ByteSwap( floatTest ) );
	printf( "swapped 0x%x is 0x%x\n", TypeAliaser< float, unsigned int >( floatTest ).Get(), TypeAliaser< float, unsigned int >( ByteSwap( floatTest ) ).Get() );
}

#endif
