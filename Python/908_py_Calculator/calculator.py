'''
Calculator module. Use only for test purpose.
Created on 30.11.2017
@author: Tomasz Szkudlarek
'''

def my_sum(var1=0, var2=0.0):
    ''' Function reruns sum of two arguments.
        @param: var1 - first argument
        @param: var2 - second argument
        @return: sum of var1 and var2
    '''
    
    try:
        if var1 == 0 and var2 == 0:
            return 1
        return var1 + var2
    except:
        return 'ERROR: {0}+{1}  I do not know how to add it'.format(var1, var2)
    
def my_sub(var1=0, var2=0):
    ''' Function reruns subtraction of two arguments.
        @param: var1 - first argument
        @param: var2 - second argument
        @return: sub of var1 and var2
    '''
    
    try:
        return var1 - var2
    except:
        return 'ERROR: {0}-{1}  I do not know how to sub it'.format(var1, var2)
    

def my_mul(var1=0, var2=0):
    ''' Function reruns multiplication of two arguments.
        @param: var1 - first argument
        @param: var2 - second argument
        @return: multiplication of var1 and var2
    '''
    
    try:
        return var1 * var2
    except:
        return 'ERROR: {0}*{1}  I do not know how to multiply it'.format(var1, var2)


def my_div(var1=0, var2=1):
    ''' Function reruns multiplication of two arguments.
        @param: var1 - first argument
        @param: var2 - second argument
        @return: division of var1 and var2
    '''
    
    try:
        if var2 == 0:
            return 0
        else:
            return var1 / var2
    except:
        return 'ERROR: {0}/{1}  I do not know how to divide it'.format(var1, var2)


if __name__ == '__main__':
    print(my_sum('dsf', 8))
    print(my_sub('d', 55))
    print(my_mul('f', 13))
    print(my_div(4, 's'))
    