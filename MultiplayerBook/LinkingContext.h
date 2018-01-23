
#ifndef RoboCat_LinkingContext_h
#define RoboCat_LinkingContext_h

class GameObject;

class LinkingContext
{
public:
	
	LinkingContext() : 
	mNextNetworkId( 1 )
	{}

	unsigned int GetNetworkId( GameObject* inGameObject, bool inShouldCreateIfNotFound )
	{
		auto it = mGameObjectToNetworkIdMap.find( inGameObject );
		if( it != mGameObjectToNetworkIdMap.end() )
		{
			return it->second;
		}
		else if( inShouldCreateIfNotFound )
		{
			unsigned int newNetworkId = mNextNetworkId++;
			AddGameObject( inGameObject, newNetworkId );
			return newNetworkId;
		}
		else
		{
			return 0;
		}
	}
	
	GameObject* GetGameObject( unsigned int inNetworkId ) const
	{
		auto it = mNetworkIdToGameObjectMap.find( inNetworkId );
		if( it != mNetworkIdToGameObjectMap.end() )
		{
			return it->second;
		}
		else
		{
			return nullptr;
		}
	}

	void AddGameObject( GameObject* inGameObject, unsigned int inNetworkId )
	{
		mNetworkIdToGameObjectMap[ inNetworkId ] = inGameObject;
		mGameObjectToNetworkIdMap[ inGameObject ] = inNetworkId;
	}
	
	void RemoveGameObject( GameObject *inGameObject )
	{
		unsigned int networkId = mGameObjectToNetworkIdMap[ inGameObject ];
		mGameObjectToNetworkIdMap.erase( inGameObject );
		mNetworkIdToGameObjectMap.erase( networkId );
	}
	
private:
	std::unordered_map< unsigned int, GameObject* > mNetworkIdToGameObjectMap;
	std::unordered_map< const GameObject*, unsigned int > mGameObjectToNetworkIdMap;

	unsigned int mNextNetworkId;
};

#endif
