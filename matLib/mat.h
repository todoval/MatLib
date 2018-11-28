#pragma once
#include <memory>
#include <vector>
#include <exception>
#include <iostream>
#include <type_traits>


class aMatrix
{
public:
	virtual int getHeight() { return -1; };
	virtual int getWidth() { return -1; };
};

template<typename T>
class simpleMat : public aMatrix //matrices diagonal or filled
{
public:
	T value;
	bool isDiagonal; //else is filled
	int width;
	int height;
	simpleMat(int _height, int _width, T _value, bool _isDiagonal) : value(_value), isDiagonal(_isDiagonal), height(_height), width(_width)
	{
	}

	int getHeight()
	{
		return height;
	}

	int getWidth()
	{
		return width;
	}

};

template<typename T>
class generalMat : public aMatrix //other matrices
{
public: 
	generalMat(std::vector<std::vector<T>> tempMat) : fullMatrix(tempMat) 
	{
	}
	std::vector<std::vector<T>> fullMatrix;

	int getHeight()
	{
		return fullMatrix.size();
	}

	int getWidth()
	{
		return fullMatrix[0].size();
	}
};

template<typename T>
class mat
{
public:
	std::shared_ptr<aMatrix> matrix;
	static mat<T> filled(size_t height, size_t width, T value);
	static mat<T> diagonal(size_t size, T value);
	static mat<T> zero(size_t height, size_t width);
	static mat<T> identity(size_t size);
	mat<T> transposition();
	mat(size_t height, size_t width);
	mat(const mat<T>& mat1);
	mat(simpleMat<T> _smp);
	mat(generalMat<T> _gen);
	~mat();
	T get(size_t row, size_t col);
	void set(size_t row, size_t col, T value);
	size_t height();
	size_t width();
	mat<T> operator+=(mat<T>& other);
	mat<T> operator-=(mat<T>& other);
	mat<T> operator*=(mat<T>& other);
};

//convert simple (diagonal or filled) matrix to general
template<typename T>
generalMat<T> convert(const simpleMat<T> &tempMat) 
{
	std::vector<std::vector<T>> toRet(0);
	for (size_t i = 0; i < tempMat.height; i++)
	{
		std::vector<T> _tmp;
		for (size_t j = 0; j < tempMat.width; j++)
		{
			if (tempMat.isDiagonal)
			{
				if(i==j)
					_tmp.push_back(tempMat.value);
				else
					_tmp.push_back(0);
			}
			else
			{
				_tmp.push_back(tempMat.value);
			}
		}
		toRet.push_back(_tmp);
	}
	return generalMat<T>(toRet);
}

//sum of two matrices of general type
template<typename T>
generalMat<T> addAll(const generalMat<T>& mat1, const generalMat<T>& mat2) 
{
	std::vector<std::vector<T>> toRet(0);
	for (size_t i = 0; i < mat1.fullMatrix.size(); i++)
	{
		std::vector<T> _tmp;
		for (size_t j = 0; j < mat1.fullMatrix[i].size(); j++)
		{
			_tmp.push_back(mat1.fullMatrix[i][j]+mat2.fullMatrix[i][j]);
		}
		toRet.push_back(_tmp);
	}
	return generalMat<T>(toRet);
}

//cartesian multiplication of two general matrices (first of type A x B, second of type B x A)
template<typename T>
generalMat<T> multiply(const generalMat<T>& mat1, const generalMat<T>& mat2)
{
	std::vector<std::vector<T>> toRet(0);
	for (size_t i = 0; i < mat1.fullMatrix.size(); i++)
	{
		std::vector<T> _tmp;
		for (size_t j = 0; j < mat2.fullMatrix[0].size(); j++)
		{
			int val = 0;
			for (size_t l = 0; l < mat1.fullMatrix[i].size(); l++)
			{
				val += mat1.fullMatrix[i][l] * mat2.fullMatrix[l][j];
			}
			_tmp.push_back(val);
		}
		toRet.push_back(_tmp);
	}
	return generalMat<T>(toRet);
}

//invert values of general matrix (multiply all by -1)
template<typename T>
generalMat<T> turnedValues(const generalMat<T>& gen)
{
	std::vector<std::vector<T>> toRet(0);
	for (size_t i = 0; i < gen.fullMatrix.size(); i++)
	{
		std::vector<T> _tmp;
		for (size_t j = 0; j < gen.fullMatrix[i].size(); j++)
		{
			_tmp.push_back(gen.fullMatrix[i][j]*(-1));
		}
		toRet.push_back(_tmp);
	}
	return generalMat<T>(toRet);
}

template<typename T>
mat<T> operator+(mat<T>& mat1, mat<T>& mat2) 
{
	try
	{
		if (mat1.matrix->getHeight() != mat2.matrix->getHeight() || mat1.matrix->getWidth() != mat2.matrix->getWidth())
		{
			throw std::exception("Can't add matrices of different sizes");
		}
		else
		{
			auto smpOne = std::dynamic_pointer_cast <simpleMat<T>> (mat1.matrix);
			auto genOne = std::dynamic_pointer_cast <generalMat<T>> (mat1.matrix);
			auto smpTwo = std::dynamic_pointer_cast <simpleMat<T>> (mat2.matrix);
			auto genTwo = std::dynamic_pointer_cast <generalMat<T>> (mat2.matrix);
			if (smpOne && smpTwo)
			{
				if (smpOne->isDiagonal && smpTwo->isDiagonal) //both diagonal
				{
					simpleMat<T> smpTemp(mat1.matrix->getHeight(), mat1.matrix->getHeight(), smpOne->value + smpTwo->value, true);
					mat<T> tempMat(smpTemp);
					return tempMat;
				}
				else if (!smpOne->isDiagonal && !smpTwo->isDiagonal) //both filled
				{
					simpleMat<T> smpTemp(mat1.matrix->getHeight(), mat1.matrix->getWidth(), smpOne->value + smpTwo->value, false);
					mat<T> tempMat(smpTemp);
					return tempMat;
				}
				else if (!smpOne->isDiagonal && smpTwo->isDiagonal) //first filled second diagonal
				{
					generalMat<T> first = convert(*smpOne);
					for (size_t i = 0; i < first.fullMatrix.size(); i++)
					{
						first.fullMatrix[i][i] += smpTwo->value;
					}
					mat<T> toRet(first);
					return toRet;
				}
				else //second filled first diagonal
				{
					generalMat<T> first = convert(*smpTwo);
					for (size_t i = 0; i < first.fullMatrix.size(); i++)
					{
						first.fullMatrix[i][i] += smpOne->value;
					}
					mat<T> toRet(first);
					return toRet;
				}
			}
			else if (smpOne && genTwo)
			{
				generalMat<T> converted = convert(*smpOne);
				return mat<T>(addAll(converted, *genTwo));
			}
			else if (genOne && smpTwo)
			{
				generalMat<T> converted = convert(*smpTwo);
				return mat<T>(addAll(converted, *genOne));
			}
			else if (genOne && genTwo)
			{
				return mat<T>(addAll(*genOne, *genTwo));
			}
		}
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	return mat1;
};

template<typename T>
mat<T> operator-(mat<T>& mat1, mat<T>& mat2)
{
	try
	{
		if (mat1.matrix->getHeight() != mat2.matrix->getHeight() || mat1.matrix->getWidth() != mat2.matrix->getWidth())
		{
			throw std::exception("Can't subtract matrices of different sizes");
		}
		else
		{
			auto smpOne = std::dynamic_pointer_cast <simpleMat<T>> (mat1.matrix);
			auto genOne = std::dynamic_pointer_cast <generalMat<T>> (mat1.matrix);
			auto smpTwo = std::dynamic_pointer_cast <simpleMat<T>> (mat2.matrix);
			auto genTwo = std::dynamic_pointer_cast <generalMat<T>> (mat2.matrix);
			if (smpOne && smpTwo)
			{
				if (smpOne->isDiagonal && smpTwo->isDiagonal) //both diagonal
				{
					simpleMat<T> smpTemp(mat1.matrix->getHeight(), mat1.matrix->getHeight(), smpOne->value - smpTwo->value, true);
					mat<T> tempMat(smpTemp);
					return tempMat;
				}
				else if (!smpOne->isDiagonal && !smpTwo->isDiagonal) //both filled
				{
					simpleMat<T> smpTemp(mat1.matrix->getHeight(), mat1.matrix->getWidth(), smpOne->value - smpTwo->value, false);
					mat<T> tempMat(smpTemp);
					return tempMat;
				}
				else if (!smpOne->isDiagonal && smpTwo->isDiagonal) //first filled second diagonal
				{
					generalMat<T> first = convert(*smpOne);
					for (size_t i = 0; i < first.fullMatrix.size(); i++)
					{
						first.fullMatrix[i][i] -= smpTwo->value;
					}
					mat<T> toRet(first);
					return toRet;
				}
				else //second filled first diagonal
				{
					generalMat<T> second = turnedValues(convert(*smpTwo));
					for (size_t i = 0; i < second.fullMatrix.size(); i++)
					{
						second.fullMatrix[i][i] += smpOne->value;
					}
					mat<T> toRet(second);
					return toRet;
				}
			}
			else if (smpOne && genTwo)
			{
				generalMat<T> converted = convert(*smpOne);
				return mat<T>(addAll(converted, turnedValues(*genTwo)));
			}
			else if (genOne && smpTwo)
			{
				generalMat<T> converted = turnedValues(convert(*smpTwo));
				return mat<T>(addAll(*genOne, converted));
			}
			else if (genOne && genTwo)
			{
				generalMat<T> turned = turnedValues(*genTwo);
				return mat<T>(addAll(*genOne, turned));
			}
		}
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	return mat1;
};

template<typename T>
mat<T> operator*(mat<T>& mat1, mat<T>& mat2)
{
	try
	{
		if (mat1.matrix->getWidth() != mat2.matrix->getHeight())
		{
			throw std::exception("Can't multiply matrices");
		}
		else
		{
			auto smpOne = std::dynamic_pointer_cast <simpleMat<T>> (mat1.matrix);
			auto genOne = std::dynamic_pointer_cast <generalMat<T>> (mat1.matrix);
			auto smpTwo = std::dynamic_pointer_cast <simpleMat<T>> (mat2.matrix);
			auto genTwo = std::dynamic_pointer_cast <generalMat<T>> (mat2.matrix);
			if (smpOne && smpTwo)
			{
				if (smpOne->isDiagonal && smpTwo->isDiagonal) //both diagonal
				{
					simpleMat<T> smpTemp(mat1.matrix->getHeight(), mat1.matrix->getHeight(), smpOne->value * smpTwo->value, true);
					mat<T> tempMat(smpTemp);
					return tempMat;
				}
				else if (!smpOne->isDiagonal && !smpTwo->isDiagonal) //both filled
				{
					T valueToAdd = (smpOne->value * smpTwo->value) * mat1.matrix->getWidth();
					simpleMat<T> smpTemp(mat1.matrix->getHeight(), mat2.matrix->getWidth(), valueToAdd, false);
					mat<T> tempMat(smpTemp);
					return tempMat;
				}
				else if (!smpOne->isDiagonal && smpTwo->isDiagonal) //first filled second diagonal
				{
					T valueToAdd = smpOne->value * smpTwo->value;
					simpleMat<T> smpTemp(mat1.matrix->getHeight(), mat2.matrix->getWidth(), valueToAdd, false);
					mat<T> tempMat(smpTemp);
					return tempMat;
				}
				else //second filled first diagonal
				{
					T valueToAdd = smpOne->value* smpTwo->value;
					simpleMat<T> smpTemp(mat1.matrix->getHeight(), mat2.matrix->getWidth(), valueToAdd, false);
					mat<T> tempMat(smpTemp);
					return tempMat;
				}
			}
			else if (smpOne && genTwo)
			{
				generalMat<T> converted = convert(*smpOne);
				return mat<T>(multiply(converted, *genTwo));
			}
			else if (genOne && smpTwo)
			{
				generalMat<T> converted = convert(*smpTwo);
				return mat<T>(multiply(converted, *genOne));
			}
			else if (genOne && genTwo)
			{
				return mat<T>(multiply(*genOne, *genTwo));
			}
		}
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	return mat1;
};

template<typename T>
bool operator==(mat<T>& mat1, mat<T>& mat2)
{
	if (!mat1.matrix || !mat2.matrix)
		return false;
	if (mat2.matrix->getHeight() != mat1.matrix->getHeight() || mat2.matrix->getWidth() != mat1.matrix->getWidth())
		return false;
	auto smpThis = std::dynamic_pointer_cast <simpleMat<T>> (mat1.matrix);
	auto genThis = std::dynamic_pointer_cast <generalMat<T>> (mat1.matrix);
	auto smpOther = std::dynamic_pointer_cast <simpleMat<T>> (mat2.matrix);
	auto genOther = std::dynamic_pointer_cast <generalMat<T>> (mat2.matrix);
	if (smpOther && smpThis) //both simple
	{
		if (smpOther->isDiagonal && smpThis->isDiagonal && smpOther->value == smpThis->value)
			return true;
		else if (!smpThis->isDiagonal && !smpOther->isDiagonal && smpThis->value == smpOther->value)
			return true;
		else if (smpThis->isDiagonal && !smpOther->isDiagonal && smpThis->value == smpOther->value && smpThis->getHeight() == 1)
			return true;
		else if (!smpThis->isDiagonal && smpOther->isDiagonal && smpThis->value == smpOther->value && smpThis->getHeight() == 1)
			return true;
		else return false;
	}
	if (smpThis && genOther) //first simple second general
	{
		generalMat<T> converted = convert(*smpThis);
		for (size_t i = 0; i < converted.fullMatrix.size(); i++)
		{
			for (size_t j = 0; j < converted.fullMatrix[i].size(); j++)
			{
				if (converted.fullMatrix[i][j] != genOther->fullMatrix[i][j])
					return false;
			}
		}
		return true;
	}
	if (smpOther && genThis) //first general second simple
	{
		generalMat<T> converted = convert(*smpOther);
		for (size_t i = 0; i < converted.fullMatrix.size(); i++)
		{
			for (size_t j = 0; j < converted.fullMatrix[i].size(); j++)
			{
				if (converted.fullMatrix[i][j] != genThis->fullMatrix[i][j])
					return false;
			}
		}
		return true;
	}
	if (genOther && genThis) //both general
	{
		for (size_t i = 0; i < genThis->fullMatrix.size(); i++)
		{
			for (size_t j = 0; j < genThis->fullMatrix[i].size(); j++)
			{
				if (genThis->fullMatrix[i][j] != genOther->fullMatrix[i][j])
					return false;
			};
		};
	}
	return true;
}

template<typename T>
bool operator!=(mat<T>& mat1, mat<T>& mat2)
{
	return !(mat1==mat2);
}

template<typename T>
inline mat<T> mat<T>::operator+=(mat<T>& other)
{
	*this = *this + other;
	return *this;
}

template<typename T>
inline mat<T> mat<T>::operator-=(mat<T>& other)
{
	*this = *this - other;
	return *this;
}

template<typename T>
inline mat<T> mat<T>::operator*=(mat<T>& other)
{
	*this = *this * other;
	return *this;
}

//create filled matrix (simple)
template<typename T>
inline mat<T> mat<T>::filled(size_t height, size_t width, T value)
{
	simpleMat<T> smpTemp(height, width, value, false);
	mat<T> tempMat(smpTemp);
	return tempMat;
}

//create simple diagonal matrix
template<typename T>
inline mat<T> mat<T>::diagonal(size_t size, T value)
{
	simpleMat<T> smpTemp(size, size, value, true);
	mat<T> tempMat(smpTemp);
	return tempMat;
}

//create simple filled matrix with zero values
template<typename T>
inline mat<T> mat<T>::zero(size_t height, size_t width)
{
	return filled(height, width, 0);
}

//create simple diagonal matrix with values 1
template<typename T>
inline mat<T> mat<T>::identity(size_t size)
{
	return diagonal(size, 1);
}

template<typename T>
inline mat<T> mat<T>::transposition()
{
	auto smpOne = std::dynamic_pointer_cast <simpleMat<T>> (matrix);
	auto genOne = std::dynamic_pointer_cast <generalMat<T>> (matrix);
	if (smpOne)
	{
		if (smpOne->isDiagonal) return mat<T>(*smpOne);
		if (!smpOne->isDiagonal)
		{
			//is filled
			simpleMat<T> smpTemp(smpOne->width, smpOne->height, smpOne->value, false);
			mat<T> toRet(smpTemp);
			return toRet;
		}
	}
	if (genOne)
	{
		std::vector<std::vector<T>> toRet(0);
		for (int i = 0; i < genOne->getWidth(); i++)
		{
			std::vector<T> _tmp;
			for (int j = 0; j < genOne->getHeight(); j++)
			{
				_tmp.push_back(genOne->fullMatrix[j][i]);
			}
			toRet.push_back(_tmp);
		}
		return mat<T>(toRet);
	}
	return genOne->fullMatrix;
}

//constructor for a new zero matrix
template<typename T>
inline mat<T>::mat(size_t height, size_t width)
{
	try
	{
		if (!std::is_arithmetic<T>::value)
			throw std::exception("Type must be arithmetic");
		if (width <= 0)
		{
			width = 1;
		}
		if (height <= 0)
		{
			height = 1;
		}
		std::vector<std::vector<T>> tempMat(0);
		for (size_t i = 0; i < height; i++)
		{
			std::vector<T> _tmp;
			for (size_t j = 0; j < width; j++)
			{
				_tmp.push_back(0);
			}
			tempMat.push_back(_tmp);
		}
		matrix = std::make_shared<generalMat<T>>(generalMat<T>(tempMat));
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}

//copy constructor
template<typename T>
inline mat<T>::mat(const mat<T>& mat1)
{
	matrix = mat1.matrix;
}

//constructor for simple matrices
template<typename T>
inline mat<T>::mat(simpleMat<T> _smp)
{
	matrix = std::make_shared<simpleMat<T>>(_smp);
}

//constructor for general matrices
template<typename T>
inline mat<T>::mat(generalMat<T> _gen)
{
	matrix = std::make_shared<generalMat<T>>(_gen);
}

template<typename T>
inline mat<T>::~mat()
{
}

template<typename T>
inline T mat<T>::get(size_t row, size_t col)
{
	try
	{
		auto smp = std::dynamic_pointer_cast <simpleMat<T>> (matrix);
		auto gen = std::dynamic_pointer_cast <generalMat<T>> (matrix);
		if (smp)
		{
			if (row > smp->height || col > smp->width)
				throw std::exception("Out of bounds");
			else if (smp->isDiagonal && row == col)
				return smp->value;
			else return 0;
		}
		else
		{
			if (row > gen->fullMatrix.size() || col > gen->fullMatrix[0].size())
			{
				throw std::exception("Out of bounds");
			}
			int x = 0;
			int y = 0;
			for (auto i = gen->fullMatrix.begin(); i < gen->fullMatrix.end(); i++)
			{
				for (auto j = i->begin(); j < i->end(); j++)
				{
					if ((x == row) && (y == col))
						return *j;
					++y;
				};
				y = 0;
				++x;
			};
		}
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	return -1;
}

template<typename T>
inline void mat<T>::set(size_t row, size_t col, T value)
{
	try
	{
		if (row > matrix->getHeight() || row < 0 || col < 0 || col > matrix->getWidth())
		{
			throw std::exception("Out of bounds");
			return;
		}
		auto smp = std::dynamic_pointer_cast <simpleMat<T>> (matrix);
		auto gen = std::dynamic_pointer_cast <generalMat<T>> (matrix);
		if (smp)
		{
			std::vector<std::vector<T>> tempMat(0);
			for (size_t i = 0; i < smp->height; i++)
			{
				std::vector<T> _tmp;
				for (size_t j = 0; j < smp->width; j++)
				{
					if (smp->isDiagonal)
					{
						if (i == j)
						{
							_tmp.push_back(smp->value);
						}
						else _tmp.push_back(0);
					}
					else
					{
						_tmp.push_back(smp->value);
					}
				}
				tempMat.push_back(_tmp);
			}
			tempMat[row][col] = value;
			matrix = std::make_shared<generalMat<T>>(generalMat<T>(tempMat));
		}
		else
		{
			if (matrix.use_count() > 1)
			{
				auto tempMat = *gen;
				tempMat.fullMatrix[row][col] = value;
				matrix = std::make_shared<generalMat<T>>(generalMat<T>(tempMat));
			}
			else
			{
				gen->fullMatrix[row][col] = value;
			}
		}
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}

template<typename T>
inline size_t mat<T>::height()
{
	auto smpOne = std::dynamic_pointer_cast <simpleMat<T>> (matrix);
	auto genOne = std::dynamic_pointer_cast <generalMat<T>> (matrix);
	if (smpOne) return smpOne->getHeight();
	if (genOne) return genOne->fullMatrix.size();
	return 0;
}

template<typename T>
inline size_t mat<T>::width()
{
	auto smpOne = std::dynamic_pointer_cast <simpleMat<T>> (matrix);
	auto genOne = std::dynamic_pointer_cast <generalMat<T>> (matrix);
	if (smpOne) return smpOne->getWidth();
	if (genOne) return genOne->fullMatrix[0].size();
	return 0;
}
