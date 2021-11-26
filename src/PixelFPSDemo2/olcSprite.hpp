#pragma once

#include <string>
using namespace std;

class olcSprite
{
public:
    int nWidth = 0;
    int nHeight = 0;

    olcSprite()
    {

    }

    olcSprite(int w, int h)
    {
        Create(w, h);
    }

    olcSprite(const std::wstring& sFile)
    {
        if (!Load(sFile))
            Create(8, 8);
    }

private:
    short* m_Glyphs = nullptr;
    short* m_Colours = nullptr;

    void Create(int w, int h)
    {
        nWidth = w;
        nHeight = h;
        m_Glyphs = new short[w * h];
        m_Colours = new short[w * h];
        for (int i = 0; i < w * h; i++)
        {
            m_Glyphs[i] = L' ';
            m_Colours[i] = 0;
        }
    }

public:
    void SetGlyph(int x, int y, short c)
    {
        if (x < 0 || x >= nWidth || y < 0 || y >= nHeight)
            return;
        else
            m_Glyphs[y * nWidth + x] = c;
    }

    void SetColour(int x, int y, short c)
    {
        if (x < 0 || x >= nWidth || y < 0 || y >= nHeight)
            return;
        else
            m_Colours[y * nWidth + x] = c;
    }

    short GetGlyph(int x, int y)
    {
        if (x < 0 || x >= nWidth || y < 0 || y >= nHeight)
            return L' ';
        else
            return m_Glyphs[y * nWidth + x];
    }

    short GetColour(int x, int y)
    {
        if (x < 0 || x >= nWidth || y < 0 || y >= nHeight)
            return 0;
        else
            return m_Colours[y * nWidth + x];
    }

    short SampleGlyph(float x, float y)
    {
        int sx = (int)(x * (float)nWidth);
        int sy = (int)(y * (float)nHeight - 1.0f);
        if (sx < 0 || sx >= nWidth || sy < 0 || sy >= nHeight)
            return L' ';
        else
            return m_Glyphs[sy * nWidth + sx];
    }

    short SampleColour(float x, float y)
    {
        int sx = (int)(x * (float)nWidth);
        int sy = (int)(y * (float)nHeight - 1.0f);
        if (sx < 0 || sx >= nWidth || sy < 0 || sy >= nHeight)
            return 0;
        else
            return m_Colours[sy * nWidth + sx];
    }

    bool Save(std::wstring sFile)
    {
        FILE* f = nullptr;
        _wfopen_s(&f, sFile.c_str(), L"wb");
        if (f == nullptr)
            return false;

        fwrite(&nWidth, sizeof(int), 1, f);
        fwrite(&nHeight, sizeof(int), 1, f);
        fwrite(m_Colours, sizeof(short), nWidth * nHeight, f);
        fwrite(m_Glyphs, sizeof(short), nWidth * nHeight, f);

        fclose(f);

        return true;
    }

    bool Load(const std::wstring& sFile)
    {
        delete[] m_Glyphs;
        delete[] m_Colours;
        nWidth = 0;
        nHeight = 0;

        FILE* f = nullptr;
        _wfopen_s(&f, sFile.c_str(), L"rb");
        if (f == nullptr)
            return false;

        std::fread(&nWidth, sizeof(int), 1, f);
        std::fread(&nHeight, sizeof(int), 1, f);

        Create(nWidth, nHeight);

        std::fread(m_Colours, sizeof(short), nWidth * nHeight, f);
        std::fread(m_Glyphs, sizeof(short), nWidth * nHeight, f);

        std::fclose(f);
        return true;
    }

    //new function resize
    void Resize(int newWidth, int newHeight)
    {
        short* newGlyphs = new short[newWidth * newHeight];
        short* newColours = new short[newWidth * newHeight];

        for (int y = 0; y < newHeight; y++)
        {
            for (int x = 0; x < newWidth; x++)
            {
                newGlyphs[y * newWidth + x] = this->GetGlyph(x, y);
                newColours[y * newWidth + x] = this->GetColour(x, y);
            }
        }

        delete[] this->m_Glyphs;
        delete[] this->m_Colours;

        this->m_Glyphs = newGlyphs;
        this->m_Colours = newColours;

        this->nWidth = newWidth;
        this->nHeight = newHeight;
    }
};