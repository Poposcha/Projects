# -*- coding: utf-8 -*-

import scrapy
import pycountry
from locations.items import GeojsonPointItem
from locations.categories import Code
from typing import List, Dict
import requests


class TododiaSpider(scrapy.Spider):
    name = 'tododia_bra_dac'
    brand_name = 'TodoDia'
    spider_type: str = 'chain'
    spider_chain_id: str = "23685"
    spider_categories: List[str] = [Code.GROCERY]
    spider_countries: List[str] = [pycountry.countries.lookup('bra').alpha_3]
    allowed_domains: List[str] = ['grupobigbrasil.com.br']

    def start_requests(self):
        '''
        Spider entrypoint.
        Request chaining starts from here.
        '''
        url: str = "https://tabloide.big.com.br/api/v.5/lojas"
        yield scrapy.FormRequest(
            url=url,
            method='POST',
            formdata={"bandeiras": "8,15,13,6,5"},
            callback=self.parse,
        )

    def parse(self, response) -> GeojsonPointItem:
        stores = response.json()['lojas']
        for row in stores:
            if "tododia" in row.get('slug'):
                data = {
                    "brand": self.brand_name,
                    "chain_id": self.spider_chain_id,
                    "chain_name": self.brand_name,
                    "ref": row.get('id'),
                    "name": row.get('nome'),
                    "addr_full": row.get("endereco"),
                    "country": "Brazil",
                    "city": row.get('cidade'),
                    "postcode": row.get('sep'),
                    "website": "https://www.grupobigbrasil.com.br/",
                    "phone": row.get("telefone"),
                    "lat": "" if row.get('latitude') is None else float(row.get('latitude')),
                    "lon": "" if row.get('longitude') is None else float(row.get('longitude')),
                }
                yield GeojsonPointItem(**data)




# url = "https://tabloide.big.com.br/api/v.5/lojas"

# response = requests.post(url, data={"bandeiras" : "8,15,13,6,5"})
# print(response.status_code)
# print(response.json())



