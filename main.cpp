/* ****************************************
Описание использования

Пользователь вводит линейное уравнение вида a+b*x=c, где
a,b,c -- любое выражение, содержащее числа и операторы умножения и деления.
Программа рассчитывает величину x и выводит её на печать.

Описание алгоритма

Считва строку программа парсит её по следующему алгоритму:
1. нахождение левой и правой частей уравнения
2. работа с левой частью
2.1 находим положение неизвестной и множитель b перед ней;
2.2 заменяем [+-]b*x на +0 в левой части
2.2.1 заменяем x на единицу в подстроке вида [+-]b*x
2.2.2 рассчитваем b
2.3 оставшееся выражение рассчитываем с учетом приоритета операций и получаем значение a
3. работа с правой частью
3.3 рассчитываем выражение с учетом приоритета операций
4. находим значение неизвестной x=(c-a)/b

Замечания

Деление на x не предусмотренно.
Не должно быть удвоенных минусов.
 **************************************** */

#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <numeric>

using namespace std;

namespace equation
{
    class linear
    {
        private:
            /* Линейное уравнение хранится в eq */
            std::string eq;
            double a = 0, b = 0, c = 0;
        private:
            /* Вычисляет значение строкового численного выражения */
            double calc(const string& arg)
            {
                /* Идея
                Делим входные данные на два массива, в первом -- операнды,
                во втором -- операторы. Далее группируем операнды, которые
                соответствуют операции умножения либо сложения, парами и
                вычисляем значение операции. Результат возвращаем на место
                пары n*m в массиве операндов. Делаем это до тех пор, пока
                не закочатся операции умножения и деления. Сложив весь
                преобразованный массив оперндов, получаем результат выражения
                arg */
                /* Вычисляем выражение */
                vector<double>      num;
                string              op;
                string line =       arg;
                for(size_t i = 0; i < line.length(); ++i)
                {
                    /* Находим операнды и пушим их в стек чисел */
                    try
                    { /* Исключение бросает stod в случае, если  первый аргумент не число */
                        size_t endPos = 0; // Конец числа
                        num.push_back(stod(line.substr(i), &endPos));
                        i = i + endPos - 1;

                        if(i != line.length() - 1)
                            op += '+';
                    }
                    catch(const invalid_argument&e)
                    {
                        /* Находим операторы и пушим их в стэк операторов */
                        if(line[i] == '*' || line[i] == '/')
                        {
                            op[op.size() - 1] = line[i];
                        }
                    }
                }
                /* Калькулируем */
                while(true)
                {
                    size_t pos = 0;

                    pos = op.find('*');
                    if(pos == string::npos)
                    {
                        pos = op.find('/');
                        if(pos == string::npos)
                            break;
                        else
                            num[pos+1] = num[pos]/num[pos+1];
                    }
                    else
                        num[pos+1] = num[pos]*num[pos+1];

                    op.erase(pos, 1);
                    num.erase(num.begin() + pos);
                }
                double result = 0;
                result = accumulate(num.begin(), num.end(), result);

                //cout << "R: " << result << endl;
                return result;
            }
        public:
            explicit linear(const std::string arg)
            : eq{arg}
            {
            }
            double resolve() const
            {
                /* 4. находим значение неизвестной x=(c-a)/b */
                return (c - a) / b;
            }
            bool parse()
            {
                /* 1. нахождение левой и правой частей уравнения */
                size_t eqPos = eq.find('=');
                if(eqPos == string::npos)
                {
                    return false;
                }
                //cout << eqPos << endl;
                string strL{eq, 0, eqPos};
                string strR{eq, eqPos+1, eq.length() - eqPos};
                //cout << strL << "|" << strR << endl;
                /* 2. работа с левой частью */
                /*2.1 находим положение неизвестной и множитель b перед ней; */
                size_t xPos = strL.find('x');
                size_t subStrPosBegin = xPos;
                while(subStrPosBegin > 0)
                {
                    if(strL[subStrPosBegin] == '+' || strL[subStrPosBegin] == '-')
                        break;
                    subStrPosBegin--;
                }
                size_t subStrPosEnd = xPos;
                while(subStrPosEnd < strL.length())
                {
                    if(strL[subStrPosEnd] == '+' || strL[subStrPosEnd] == '-')
                        break;
                    subStrPosEnd++;
                }
                //cout << subStrPosBegin << " " << subStrPosEnd;
                string subStr {strL, subStrPosBegin, subStrPosEnd-subStrPosBegin};
                /* 2.2 заменяем [+-]b*x на +0 в левой части */
                strL.replace(subStrPosBegin, subStr.length(), "+0");
                /*2.2.1 заменяем x на единицу в подстроке вида [+-]b*x */
                //cout << subStr << endl;
                //cout << strL;
                subStr.replace(subStr.find('x'), 1, "1");
                /*2.2.2 рассчитваем b */
                b = calc(subStr);
                /* 2.3 оставшееся выражение рассчитываем с учетом приоритета операций и получаем значение a*/
                a = calc(strL);
                /* 3. работа с правой частью */
                /* 3.3 рассчитываем выражение с учетом приоритета операций */
                c = calc(strR);

                return true;
            }
    };
}

int main()
{
    while(true)
    {
        string input = "1+1*5*x*2=12/2";
        cout << "Enter your equation: ";
        cin >> input;

        equation::linear myLinearEq{input};
        if(!myLinearEq.parse())
        {
            cerr << "Parsing error!" << endl;
            return -1;
        }
        else
        {
            double result = myLinearEq.resolve();
            cout << "Result is " << result << endl;
        }
    }
    return 0;
}
