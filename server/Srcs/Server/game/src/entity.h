#ifndef __INC_METIN_II_GAME_ENTITY_H__
#define __INC_METIN_II_GAME_ENTITY_H__
#include "../../common/stl.h"

class SECTREE;

class CEntity
{
	public:
		typedef TR1_NS::unordered_map<LPENTITY, int> ENTITY_MAP;

	public:
		CEntity();
		virtual	~CEntity();

		virtual void	EncodeInsertPacket(LPENTITY entity) = 0;
		virtual	void	EncodeRemovePacket(LPENTITY entity) = 0;

	protected:
		void			Initialize(int type = -1);
		void			Destroy();

	public:
		void			SetType(int type);
		int				GetType() const;
		bool			IsType(int type) const;

		void			ViewCleanup(
			#ifdef ENABLE_GOTO_LAG_FIX
			bool recursive = false
			#endif
		);
		void			ViewInsert(LPENTITY entity, bool recursive = true);
		void			ViewRemove(LPENTITY entity, bool recursive = true);
		void			ViewReencode();

		int				GetViewAge() const	{ return m_iViewAge;	}

		long			GetX() const		{ return m_pos.x; }
		long			GetY() const		{ return m_pos.y; }
		long			GetZ() const		{ return m_pos.z; }
		const PIXEL_POSITION &	GetXYZ() const		{ return m_pos; }

		void			SetXYZ(long x, long y, long z)		{ m_pos.x = x, m_pos.y = y, m_pos.z = z; }
		void			SetXYZ(const PIXEL_POSITION & pos)	{ m_pos = pos; }

		LPSECTREE		GetSectree() const			{ return m_pSectree;	}
		void			SetSectree(LPSECTREE tree)	{ m_pSectree = tree;	}
		void			UpdateSectree();

		void			PacketAround(const void * data, int bytes, LPENTITY except = NULL);
		void			PacketView(const void * data, int bytes, LPENTITY except = NULL);


		template<typename T, std::enable_if_t<utils::IsRawV<T>>* = nullptr>
		void PacketAround(const T& c_pvData, LPENTITY except = nullptr) {
			PacketAround(&c_pvData, sizeof(T), except);
		}
		template<typename C, std::enable_if_t<utils::IsContiguousV<C>>* = nullptr>
		void PacketAround(const C& v, LPENTITY except = nullptr) {
			PacketAround(v.data(), v.size() * sizeof(typename C::value_type), except);
		}

		template<typename T, std::enable_if_t<utils::IsRawV<T>>* = nullptr>
		void PacketView(const T& c_pvData, LPENTITY except = nullptr) {
			PacketView(&c_pvData, sizeof(T), except);
		}
		template<typename C, std::enable_if_t<utils::IsContiguousV<C>>* = nullptr>
		void PacketView(const C& v, LPENTITY except = nullptr) {
			PacketView(v.data(), v.size() * sizeof(typename C::value_type), except);
		}

		void			BindDesc(LPDESC _d)     { m_lpDesc = _d; }
		LPDESC			GetDesc() const			{ return m_lpDesc; }

		void			SetMapIndex(long l)	{ m_lMapIndex = l; }
		long			GetMapIndex() const	{ return m_lMapIndex; }
		int				GetRealMapIndex() const { return (m_lMapIndex >= 10000) ? (m_lMapIndex / 10000) : m_lMapIndex; }

		void			SetObserverMode(bool bFlag);
		bool			IsObserverMode() const	{ return m_bIsObserver; }

	protected:
		bool			m_bIsObserver;
		bool			m_bObserverModeChange;
		ENTITY_MAP		m_map_view;
		long			m_lMapIndex;

	private:
		LPDESC			m_lpDesc;

		int			m_iType;
		bool			m_bIsDestroyed;

		PIXEL_POSITION		m_pos;

		int			m_iViewAge;

		LPSECTREE		m_pSectree;
};

#endif
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
