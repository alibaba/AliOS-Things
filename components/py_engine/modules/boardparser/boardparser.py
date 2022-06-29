import uio as io
import ujson as json

# Json file path

class BoardConfigParser:
    
    NODE_NOT_EXIST = -4

    _instance = None

    configFile = '/data/pyamp/board.json'

    def __new__(cls, *args, **kw):
        if cls._instance is None:
            cls._instance = object.__new__(cls, *args, **kw)
        return cls._instance

    def __init__(self):
        pass

    def setConfigFile(self, configFile):
        self.configFile = configFile

    def getConfigFile(self):
        return self.configFile
    
    def findItem(self, itemName, itemType):
        if isinstance(itemType, str):
            with open(self.configFile, 'r') as f:
                boardConfig = json.loads(f.read())
                ioConfig = boardConfig['io']
                if itemName not in ioConfig.keys():
                    raise Exception('node: {} does not exist in board.json file'.format(itemName))
                item = ioConfig[itemName]
                if(item['type'] != itemType):
                    raise Exception('Type not match: expect{}, get{}'.format(itemType, item['type']))
                return item
        else:
            raise Exception('Need to set item type')
    
# parser = BoardConfigParser('board.json')
# item = parser.findItem('serial1')
# print(item)