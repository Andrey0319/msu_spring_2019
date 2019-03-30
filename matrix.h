#ifndef matrix
#define matrix

using std::cout;
using std::cin;
using std::endl;

class Matrix
{
private:
    int rows_count;
    int columns_count;
    int** mat;

public:
    Matrix(const int rows, const int columns)
    {
        if (rows < 0 || columns < 0)
            throw std::out_of_range("");
        else
        {
            rows_count = rows;
            columns_count = columns;
            mat = new int*[rows];

            for (int i = 0; i < rows;  i += 1)
                mat[i] =  new int[columns];
        }
    }

    class Row
    {
    public:
        int *mat;
        unsigned columns_count;

        Row (int *row, const int c)
            :mat (row), columns_count (c) {}

        const int& operator [] (size_t j) const
        {
            if (j >= columns_count || j < 0)
                throw std::out_of_range("");
            return mat[j];
        }

        int& operator [] (size_t j)
        {
            if (j >= columns_count || j < 0)
                throw std::out_of_range("");
            return mat[j];
        }
    };

    const Row operator [] (const size_t i) const
    {
        if (i >= rows_count || i < 0)
            throw std::out_of_range("");
        Row row (mat[i], columns_count);
        return row;
    }

    Row operator [] (const size_t i)
    {
        if (i >= rows_count || i < 0)
            throw std::out_of_range("");
        Row row (mat[i], columns_count);
        return row;
    }

    int getColumns() const
    {
        return columns_count;
    }

    int getRows() const
    {
        return rows_count;
    }

    Matrix operator=(const Matrix &another_mat) const
    {
        if (rows_count != another_mat.rows_count || columns_count != another_mat.columns_count)
        {
            throw std::out_of_range("");
        }
        else
        {
            for (int i = 0; i < rows_count; i++)
                for (int j = 0; j < columns_count; j++)
                    mat[i][j] = another_mat.mat[i][j];

            return *this;
        }
    }

    Matrix& operator*=(const int a)
    {
        for (int i = 0; i < rows_count; i++)
            for (int j = 0; j < columns_count; j++)
                mat[i][j] *= a;

        return *this;
    }

    bool operator==(const Matrix &another_mat) const
    {
        if (rows_count != another_mat.rows_count || columns_count != another_mat.columns_count)
        {
            return false;
        }
        else
        {
            bool is_equal = true;

            for (int i = 0; i < rows_count; i++)
            {
                for (int j = 0; j < columns_count; j++)
                    if (mat[i][j] != another_mat.mat[i][j])
                    {
                        is_equal = false;
                        break;
                    }
                if (!is_equal)
                    break;
            }
            return is_equal;
        }
    }

    bool operator!=(const Matrix &another_mat) const
    {
        return !(*this == another_mat);
    }

    ~Matrix()
    {
        for (int i = 0; i < rows_count; i++)
            delete [] mat[i];
        delete [] mat;
    }
};
#endif
