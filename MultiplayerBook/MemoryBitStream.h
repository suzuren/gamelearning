
#include <cstdint>
#include <cstdlib>
#include <string>
#include <cstring>

#include "RoboMath.h"

class GameObject;

inline unsigned int ConvertToFixed( float inNumber, float inMin, float inPrecision )
{
	return static_cast< int > ( ( inNumber - inMin ) / inPrecision );
}

inline float ConvertFromFixed( unsigned int inNumber, float inMin, float inPrecision )
{
	return static_cast< float >( inNumber ) * inPrecision + inMin;
}


class OutputMemoryBitStream
{
public:

	OutputMemoryBitStream() :
		mBuffer(nullptr),
		mBitHead(0)		
	{
		ReallocBuffer( 1500 * 8 );
	}

	~OutputMemoryBitStream()	{ std::free( mBuffer ); }

	void		WriteBits( uint8_t inData, uint32_t inBitCount );
	void		WriteBits( const void* inData, uint32_t inBitCount );

	const 	char*	GetBufferPtr()		const	{ return mBuffer; }
	uint32_t		GetBitLength()		const	{ return mBitHead; }
	uint32_t		GetByteLength()		const	{ return ( mBitHead + 7 ) >> 3; }

	void WriteBytes( const void* inData, unsigned int inByteCount )	{ WriteBits( inData, inByteCount << 3 ); }

	/*
	void Write( unsigned int inData, uint3unsigned int2_t inBitCount = 32 )	{ WriteBits( &inData, inBitCount ); }
	void Write( int inData, unsigned int inBitCount = 32 )		{ WriteBits( &inData, inBitCount ); }
	void Write( float inData )								{ WriteBits( &inData, 32 ); }

	void Write( uint16_t inData, unsigned int inBitCount = 16 )	{ WriteBits( &inData, inBitCount ); }
	void Write( int16_t inData, unsigned int inBitCount = 16 )	{ WriteBits( &inData, inBitCount ); }

	void Write( uint8_t inData, unsigned int inBitCount = 8 )	{ WriteBits( &inData, inBitCount ); }
	*/
	
	template< typename T >
	void Write( T inData, unsigned int inBitCount = sizeof( T ) * 8 )
	{
		static_assert( std::is_arithmetic< T >::value ||
					  std::is_enum< T >::value,
					  "Generic Write only supports primitive data types" );
		WriteBits( &inData, inBitCount );
	}
	
	void 		Write( bool inData )								{ WriteBits( &inData, 1 ); }
	
	void		Write( const Vector3& inVector );	
	void		Write( const Quaternion& inQuat );

	void Write( const std::string& inString )
	{
		unsigned int elementCount = static_cast< unsigned int >( inString.size() );
		Write( elementCount );
		for( const auto& element : inString )
		{
			Write( element );
		}
	}
	
private:
	void		ReallocBuffer( unsigned int inNewBitCapacity );

	char*		mBuffer;
	unsigned int	mBitHead;
	unsigned int	mBitCapacity;
};

class InputMemoryBitStream
{
public:
	
	InputMemoryBitStream( char* inBuffer, unsigned int inBitCount ) :
	mBuffer( inBuffer ),
	mBitHead( 0 ),
	mBitCapacity( inBitCount ),	
	mIsBufferOwner( false ) {}
	
	InputMemoryBitStream( const InputMemoryBitStream& inOther ) :
	mBitHead( inOther.mBitHead ),
	mBitCapacity( inOther.mBitCapacity ),
	mIsBufferOwner( true )
	{
		//allocate buffer of right size
		int byteCount = ( mBitCapacity + 7 ) / 8;
		mBuffer = static_cast< char* >( malloc( byteCount ) );
		//copy
		memcpy( mBuffer, inOther.mBuffer, byteCount );
	}
	
	~InputMemoryBitStream()	{ if( mIsBufferOwner ) { free( mBuffer ); }; }
	
	const 	char*	GetBufferPtr()		const	{ return mBuffer; }
	unsigned int	GetRemainingBitCount() 	const { return mBitCapacity - mBitHead; }

	void		ReadBits( uint8_t& outData, unsigned int inBitCount );
	void		ReadBits( void* outData, unsigned int inBitCount );

	void		ReadBytes( void* outData, unsigned int inByteCount )		{ ReadBits( outData, inByteCount << 3 ); }

	template< typename T >
	void Read( T& inData, unsigned int inBitCount = sizeof( T ) * 8 )
	{
		static_assert( std::is_arithmetic< T >::value ||
					  std::is_enum< T >::value,
					  "Generic Read only supports primitive data types" );
		ReadBits( &inData, inBitCount );
	}
	
	void		Read( unsigned int& outData, unsigned int inBitCount = 32 )		{ ReadBits( &outData, inBitCount ); }
	void		Read( int& outData, unsigned int inBitCount = 32 )			{ ReadBits( &outData, inBitCount ); }
	void		Read( float& outData )									{ ReadBits( &outData, 32 ); }

	void		Read( uint16_t& outData, unsigned int inBitCount = 16 )		{ ReadBits( &outData, inBitCount ); }
	void		Read( int16_t& outData, unsigned int inBitCount = 16 )		{ ReadBits( &outData, inBitCount ); }

	void		Read( uint8_t& outData, unsigned int inBitCount = 8 )		{ ReadBits( &outData, inBitCount ); }
	void		Read( bool& outData )									{ ReadBits( &outData, 1 ); }

	void		Read( Quaternion& outQuat );

	void		ResetToCapacity( unsigned int inByteCapacity )				{ mBitCapacity = inByteCapacity << 3; mBitHead = 0; }


	void Read( std::string& inString )
	{
		unsigned int elementCount;
		Read( elementCount );
		inString.resize( elementCount );
		for( auto& element : inString )
		{
			Read( element );
		}
	}

	void Read( Vector3& inVector );

private:
	char*		mBuffer;
	unsigned int	mBitHead;
	unsigned int	mBitCapacity;
	bool		mIsBufferOwner;

};

