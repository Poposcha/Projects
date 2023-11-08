# -*- coding: utf-8 -*-

import scrapy
import pycountry
from locations.items import GeojsonPointItem
from locations.categories import Code
from typing import List, Dict


class DominosSpider(scrapy.Spider):
    name = 'dominos_sa_dac'
    brand_name = 'Domino\'s'
    spider_type: str = 'chain'
    spider_chain_id: str = "27851"
    spider_categories: List[str] = [Code.FAST_FOOD, Code.TAKE_OUT_AND_DELIVERY_ONLY]
    spider_countries: List[str] = [pycountry.countries.lookup('sau').alpha_3]
    allowed_domains: List[str] = ['dominos.sa']

    def start_requests(self):
        '''
        Spider entrypoint.
        Request chaining starts from here.
        '''
        url: str = "https://order.golo03.dominos.com/store-locator-international/locate/store?regionCode=SA&latitude=25.8517475&longitude=43.52223129999993"

        yield scrapy.Request(
            url=url,
            headers={'dpz-market': 'SAUDI_ARABIA'}
        )

    def parse(self, response) -> GeojsonPointItem:
        stores = response.json()['Stores']
        for row in stores:
            data = {
                "brand": self.brand_name,
                "chain_id": self.spider_chain_id,
                "chain_name": self.brand_name,
                "ref": row.get('StoreID'),
                "name": row.get('LanguageTranslations')["en"]["StoreName"],
                "addr_full": row.get("AddressDescription"),
                "country": row.get("Region"),
                "city": row.get('City'),
                "street": row.get('StreetName'),
                "postcode": row.get('StoreID'),
                "website": "https://www.dominos.sa/",
                "phone": row.get("Phone"),
                "opening_hours": row.get("ServiceHoursDescription")["Carryout"],
                "lat": float(row.get('Latitude')),
                "lon": float(row.get('Longitude')),
            }
            yield GeojsonPointItem(**data)


"""
import requests
url = "https://order.golo03.dominos.com/store-locator-international/locate/store?regionCode=SA&latitude=25.8517475&longitude=43.52223129999993"
headers = {'dpz-market': 'SAUDI_ARABIA'}
response = requests.get(url, headers=headers)
print(response.status_code)
print(response.json()['Stores'])

"""

