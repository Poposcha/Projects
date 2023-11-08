# -*- coding: utf-8 -*-

import scrapy
import pycountry
from locations.items import GeojsonPointItem
from locations.categories import Code
from typing import List, Dict


class DominosSpider(scrapy.Spider):
    name = 'dominos_no_dac'
    brand_name = 'Domino\'s'
    spider_type: str = 'chain'
    spider_chain_id: str = "27851"
    spider_categories: List[str] = [Code.FAST_FOOD, Code.TAKE_OUT_AND_DELIVERY_ONLY]
    spider_countries: List[str] = [pycountry.countries.lookup('nor').alpha_3]
    allowed_domains: List[str] = ['dominos.no']

    def start_requests(self):
        '''
        Spider entrypoint.
        Request chaining starts from here.
        '''
        url: str = "https://www.dominos.no/api/stores"
        yield scrapy.Request(
            url=url,
        )

    def parse_opening_hours(self, hoursOfOperation):
        formatted = ""
        for day in hoursOfOperation:
            formatted += day["weekDay"][:2] + day["openingHours"] + "-" + day["closingHours"] + "; "
        formatted = formatted[:-1]

        return formatted

    def parse(self, response) -> GeojsonPointItem:
        stores = response.json()
        for row in stores:
            data = {
                "brand": self.brand_name,
                "chain_id": self.spider_chain_id,
                "chain_name": self.brand_name,
                "ref": row.get('externalId'),
                "name": row.get('name'),
                # "addr_full": row.get("AddressDescription"),
                "country": row.get("address")['country'],
                "city": row.get("address")['city'],
                "street": row.get("address")['street'],
                "postcode": row.get("address")['postalCode'],
                "website": "https://www.dominos.no/",
                "phone": row.get("localPhoneNumber"),
                "opening_hours": self.parse_opening_hours(row.get("deliveryOptions")["carryout"]["hoursOfOperation"]),
                "lat": float(row.get("address")['location']['latitude']),
                "lon": float(row.get("address")['location']['longitude']),
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
