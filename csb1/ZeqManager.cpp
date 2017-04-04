#include "ZeqManager.hpp"
#include <algorithm>
#include <iterator>
#include <thread>

namespace csb1
{
	zeroeq::Publisher* ZeqManager::_publisher = nullptr;
    zeroeq::Subscriber* ZeqManager::_subscriber = nullptr;


    

    std::function<void(int)> ZeqManager::ff;
	
    std::function<void( zeroeq::gmrv::ConstSyncGroupPtr )> ZeqManager::_receivedSyncGroupCallback;
    std::function<void( zeroeq::gmrv::ConstChangeColorGroupPtr )> ZeqManager::_changeColorUpdateCallback;
    std::function<void( zeroeq::gmrv::ConstDestroyGroupPtr )> ZeqManager::_receivedDestroyGroupCallback;
    std::function<void( zeroeq::gmrv::ConstChangeNameGroupPtr )> ZeqManager::_changeNameGroupUpdateCallback;
    std::function<void( void )> ZeqManager::_receivedSyncNeededCallback;


    static void thread_func() {
    	std::cout << "receiveEvents" << std::endl;
        ZeqManager::subscriber( )->receive( );
    }

    static std::thread th;

    /*SubscriberTimer::SubscriberTimer( void )
	{
		connect( this, SIGNAL( timeout( ) ),
			this, SLOT( receiveEvents( )));
	}

	void SubscriberTimer::receiveEvents( void )
    {
		auto subscriber = ZeqManager::subscriber( );
		while ( subscriber->receive( 0 ) );
		this->start( 50 );
    }*/

    zeroeq::Subscriber* ZeqManager::subscriber( void )
    {
      return _subscriber;
    }

	void ZeqManager::init( const std::string& session )
	{
		_publisher = new zeroeq::Publisher(
			session.empty( ) ? zeroeq::DEFAULT_SESSION : session );

		_subscriber = new zeroeq::Subscriber(
			session.empty( ) ? zeroeq::DEFAULT_SESSION : session );

#ifdef CSB1_ZEQ_USE_GMRVLEX
		_subscriber->subscribe(
			zeroeq::gmrv::SyncGroup::ZEROBUF_TYPE_IDENTIFIER( ),
			[&]( const void* data, const size_t size )
			{
                if (_receivedSyncGroupCallback)
                {
                    _receivedSyncGroupCallback(
                        zeroeq::gmrv::SyncGroup::create( data, size));
                }
			});
		_subscriber->subscribe(
			zeroeq::gmrv::ChangeColorGroup::ZEROBUF_TYPE_IDENTIFIER( ),
			[&]( const void* data, const size_t size )
			{
                if (_changeColorUpdateCallback)
                {
                    _changeColorUpdateCallback(
                        zeroeq::gmrv::ChangeColorGroup::create( data, size));
                }
			});
		_subscriber->subscribe(
			zeroeq::gmrv::DestroyGroup::ZEROBUF_TYPE_IDENTIFIER( ),
			[&]( const void* data, const size_t size )
			{
                if (_receivedDestroyGroupCallback)
                {
                    _receivedDestroyGroupCallback(
                        zeroeq::gmrv::DestroyGroup::create( data, size));
                }
			});
		_subscriber->subscribe(
			zeroeq::gmrv::ChangeNameGroup::ZEROBUF_TYPE_IDENTIFIER( ),
			[&]( const void* data, const size_t size )
			{
                if (_changeNameGroupUpdateCallback)
                {
                    _changeNameGroupUpdateCallback(
                        zeroeq::gmrv::ChangeNameGroup::create( data, size));
                }
			});
		_subscriber->subscribe(
			zeroeq::gmrv::SyncNeeded::ZEROBUF_TYPE_IDENTIFIER( ),
            [&]( const void*, const size_t )
            {
                if (_receivedSyncNeededCallback)
                {
                    _receivedSyncNeededCallback( );
                }
			});
#endif
        //_timer->start( 50 );

        th = std::thread(thread_func);
        th.join();
	}

    /*void ZeqManager::_receivedSyncGroupCallback( zeroeq::gmrv::ConstSyncGroupPtr )
	{

	}

	void ZeqManager::_changeColorUpdateCallback( zeroeq::gmrv::ConstChangeColorGroupPtr )
	{

	}

	void ZeqManager::_receivedDestroyGroupCallback( zeroeq::gmrv::ConstDestroyGroupPtr )
	{

	}

	void ZeqManager::_changeNameGroupUpdateCallback( zeroeq::gmrv::ConstChangeNameGroupPtr )
	{

	}

	void ZeqManager::_receivedSyncNeededCallback( zeroeq::gmrv::ConstSyncNeededPtr )
	{

    }*/

	void ZeqManager::publishChangeColor( const std::string& key, const unsigned int& red, 
		const unsigned int& green, const unsigned int& blue )
	{
		if( _publisher )
		{
			_publisher->publish( zeroeq::gmrv::ChangeColorGroup( key, { red, green, blue } ));
		}
	}

	void ZeqManager::publishChangeName( const std::string& key, const std::string& name )
	{
		if( _publisher )
		{
			_publisher->publish( zeroeq::gmrv::ChangeNameGroup( key, name ));
		}
	}

	void ZeqManager::publishDestroyGroup( const std::string& key )
	{
		if ( _publisher )
		{
			_publisher->publish( zeroeq::gmrv::DestroyGroup( key ) );
		}
	}

	void ZeqManager::publishSyncGroup( const std::string& key, const std::string& name, 
		const std::string& owner, const std::vector<std::string>& ids, 
		const unsigned int& red, const unsigned int& green, const unsigned int& blue )
	{
		if ( _publisher )
		{
            std::vector<unsigned int> color = { red, green, blue };

            static const std::string& delimiter = "|&|";
            std::stringstream  ss;
            std::copy(ids.begin(), ids.end(), std::ostream_iterator<std::string>(ss, delimiter.c_str()));

            _publisher->publish( zeroeq::gmrv::SyncGroup( key, name, owner, ss.str( ), color ) );
		}
	}

	void ZeqManager::publishSyncNeeded( )
	{
		if ( _publisher )
		{
            _publisher->publish( zeroeq::gmrv::SyncNeeded( ) );
		}
	}
}
