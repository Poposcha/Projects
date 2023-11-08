# -*- coding: utf-8 -*-

import scrapy
import pycountry
from locations.items import GeojsonPointItem
from locations.categories import Code
from typing import List, Dict
import re



class CafeDeCrieSpider(scrapy.Spider):
    name = 'cafe_de_crie_dac'
    brand_name = 'Cafe de Clie'
    spider_type: str = 'chain'
    spider_chain_id: str = "301105"
    spider_categories: List[str] = [Code.RESTAURANT]
    spider_countries: List[str] = [pycountry.countries.lookup('jp').alpha_2]
    allowed_domains: List[str] = ['c-united.co.jp']

    def start_requests(self):
        '''
        Spider entrypoint.
        Request chaining starts from here.
        '''
        url: str = "https://c-united.co.jp/store/request_search/?bounds=24.36820510459159,45.67631943997828,123.80894927500002,155.62535552500003"
        yield scrapy.Request(
            url=url,
        )

    def parse_opening_hours(self, hoursOfOperation, others):
        return "Mo-Su " + hoursOfOperation.replace("~", "-")
    def parse(self, response) -> GeojsonPointItem:
        stores = response.json()
        for row in stores:
            if row.get("brand_str") == "カフェ・ド・クリエ":
                data = {
                    "brand": self.brand_name,
                    "chain_id": self.spider_chain_id,
                    "chain_name": self.brand_name,
                    "ref": row.get('id'),
                    "name": row.get('name'),
                    "addr_full": row.get("address"),
                    "postcode": row.get("postal_first") + row.get("postal_last"),
                    "website": row.get("googlemap_url"),
                    "phone": row.get("tel_first") + row.get("tel_middle") + row.get("tel_last"),
                    "opening_hours": self.parse_opening_hours(row.get("business_hours"), row.get("business_hours_other")),
                    "lat": float(row.get('latitude')),
                    "lon": float(row.get('longitude')),
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
