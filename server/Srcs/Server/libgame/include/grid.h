#ifndef __INC_METIN_II_GRID_H__
#define __INC_METIN_II_GRID_H__

class CGrid
{
    public:
	CGrid(int w, int h);
	CGrid(CGrid * pkGrid, int w, int h);
	~CGrid();

	void		Clear();
	int		FindBlank(int w, int h);
	bool		IsEmpty(int iPos, int w, int h);
	bool		Put(int iPos, int w, int h);
	void		Get(int iPos, int w, int h);
	void		Print();
	unsigned int	GetSize();

    protected:
	int	m_iWidth;
	int	m_iHeight;

	char *	m_pGrid;
};

#endif
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
